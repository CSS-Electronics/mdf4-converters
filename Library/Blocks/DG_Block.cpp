#include "DG_Block.h"

#include <algorithm>
#include <climits>

#include "DT_Block.h"
#include "HD_Block.h"
#include "SD_Block.h"

namespace mdf {

    enum DG_LinkDefinitions : int {
        DG_LinkComment = 3,
        DG_LinkData = 2,
        DG_LinkNextDataGroup = 0,
        DG_LinkFirstChannelGroup = 1,
    };

    static uint64_t constexpr size = 64;
    static uint64_t constexpr linkCount = 4;

    //region Constructors

    DG_Block::DG_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_DG;
        header.linkCount = linkCount;
        header.size = size;

        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;
    }

    DG_Block::DG_Block(DG_Block const& value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy.

        // Create copies of the links.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);
        setComment(createCopyIfSet(value.getComment()));
        setDataBlock(createCopyIfSet(value.getDataBlock()));
        setFirstCG(createCopyIfSet(value.getFirstCG()));
        setNextDG(createCopyIfSet(value.getNextDG()));

        // Re-link.
        postLoad();
    }

    //endregion

    //region Inherited functions

    bool DG_Block::load(std::istream &stream) {
        // Let the base handle the initial loading.
        bool result = MDF_Block::load(stream);

        if(!result) {
            return result;
        }

        // Skip the links section.
        uint64_t currentLocation = stream.tellg();
        stream.seekg(currentLocation + header.linkCount * sizeof(uint64_t));

        // Load the data fields.
        auto* dataPtr = reinterpret_cast<char*>(&data);
        stream.read(dataPtr, sizeof(data));

        return !stream.fail();
    }

    void DG_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        links.dg_next = setLinkIfValidPointer(getNextDG());
        links.cg_first = setLinkIfValidPointer(getFirstCG());
        links.data = setLinkIfValidPointer(getDataBlock());
        links.md_comment = setLinkIfValidPointer(getComment());

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    void DG_Block::postLoad() {
        // Walk all CG blocks and register them in the set.
        std::shared_ptr<CG_Block> cgBlock = getFirstCG();
        while(cgBlock) {
            cg.insert(cgBlock);
            cgBlock = cgBlock->getNextCG();
        }

        // Walk over all the CG blocks and create a map of records IDs.
        std::map<uint64_t, int64_t> recordInformation;

        for(auto& block: cg) {
            uint64_t recordID = block->data.record_id;
            auto recordSize = static_cast<int64_t>(block->bytesUsed);
            recordInformation.emplace(std::make_pair(recordID, recordSize));
        }

        std::shared_ptr<DT_Block> dt = getDataBlock();
        dt->setupRecordInformation(recordInformation, data.dg_rec_id_size);
    }

    //endregion

    //region Getters

    std::shared_ptr<MD_Block> DG_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[DG_LinkComment]);
    }

    std::shared_ptr<DT_Block> DG_Block::getDataBlock() const {
        return std::dynamic_pointer_cast<DT_Block>(blockLinks[DG_LinkData]);
    }

    std::shared_ptr<CG_Block> DG_Block::getFirstCG() const {
        return std::dynamic_pointer_cast<CG_Block>(blockLinks[DG_LinkFirstChannelGroup]);
    }

    std::shared_ptr<DG_Block> DG_Block::getNextDG() const {
        return std::dynamic_pointer_cast<DG_Block>(blockLinks[DG_LinkNextDataGroup]);
    }

    //endregion

    //region Setters

    void DG_Block::setComment(const std::shared_ptr<mdf::MD_Block> &value) {
        blockLinks[DG_LinkComment] = value;
    }

    void DG_Block::setDataBlock(std::shared_ptr<mdf::DT_Block> const& value) {
        blockLinks[DG_LinkData] = value;
    }

    void DG_Block::setFirstCG(const std::shared_ptr<mdf::CG_Block> &value) {
        blockLinks[DG_LinkFirstChannelGroup] = value;
    }

    void DG_Block::setNextDG(std::shared_ptr<mdf::DG_Block> const& value) {
        blockLinks[DG_LinkNextDataGroup] = value;
    }

    //endregion

    //region Others

    void DG_Block::convertCGVLtoSD() {
        std::vector<std::shared_ptr<CG_Block>> blocksToRemove;

        // Loop over the CG blocks.
        for(auto const& cgBlock: cg) {
            // Check if it is a VLSD CG block.
            if(cgBlock->isVLSD()) {
                // Find the corresponding CN block.
                std::shared_ptr<CN_Block> cnBlock = findVLSDChannel(cgBlock);

                // Replace the data block with a new SD block.
                std::shared_ptr<DT_Block> dtBlock = std::make_shared<DT_Block>(*getDataBlock(), cgBlock->data.record_id);
                std::shared_ptr<SD_Block> sdBlock = std::make_shared<SD_Block>(dtBlock, cgBlock);
                cnBlock->setData(sdBlock);

                // Mark the VLSD CG block for deletion.
                blocksToRemove.push_back(cgBlock);
            }
        }

        for(auto const& cgBlock: blocksToRemove) {
            // Unlink it from the internal chain.
            for(auto const& block: cg) {
                if(block->getNextCG() == cgBlock) {
                    block->setNextCG(cgBlock->getNextCG());
                }
            }

            cg.erase(cgBlock);
        }

        // Replace the DT block with a new block, without the VLSD record information.
        // Walk over all the CG blocks and create a map of records IDs.
        std::map<uint64_t, int64_t> recordInformation;
        uint64_t bytesUsed = 0;

        for(auto& block: cg) {
            uint64_t recordID = block->data.record_id;
            auto recordSize = static_cast<int64_t>(block->bytesUsed);
            recordInformation.emplace(std::make_pair(recordID, recordSize));

            bytesUsed += block->data.cycle_count * block->bytesUsed;
        }

        // If only a single ID is known after VLSD convertion create a new DT block with that record ID, referencing
        // the original DT block.
        if(recordInformation.size() == 1) {
            auto iter = recordInformation.begin();
            uint64_t firstID = iter->first;

            std::shared_ptr<DT_Block> createdDTBlock = std::make_shared<DT_Block>(*getDataBlock(), firstID);
            createdDTBlock->header.size = sizeof(MDF_Header) + bytesUsed;
            setDataBlock(createdDTBlock);
        } else {
            std::set<uint64_t> records;

            for(auto item: recordInformation) {
                records.insert(item.first);
            }

            std::shared_ptr<DT_Block> createdDTBlock = std::make_shared<DT_Block>(*getDataBlock(), records);
            createdDTBlock->header.size = sizeof(MDF_Header) + bytesUsed;
            setDataBlock(createdDTBlock);
        }
    }

    std::shared_ptr<CN_Block> DG_Block::findVLSDChannel(std::shared_ptr<mdf::CG_Block> const& block) const {
        std::vector<std::shared_ptr<CN_Block>> channels;

        // Lambda to walk channels.
        std::function<void(std::shared_ptr<CN_Block>)> channelCallback;
        channelCallback = [&channels, &channelCallback](std::shared_ptr<CN_Block> channel) {
            if(channel->isVLSD()) {
                channels.push_back(channel);
            }

            for(auto const& cnBlock: channel->composite) {
                channelCallback(cnBlock);
            }
        };

        // Find all VLSD channels.
        for(auto const& cgBlock: cg) {
            for(auto const& cnBlock: cgBlock->cn) {
                channelCallback(cnBlock);
            }
        }

        // Find the correct block.
        std::shared_ptr<CN_Block> result;

        for(auto const& cnBlock: channels) {
            // TODO: Replace with pointer comparison.
            if(cnBlock->getDataGeneric()->fileLocation == block->fileLocation) {
                result = cnBlock;
                break;
            }
        }

        return result;
    }

    void DG_Block::addCG(std::shared_ptr<mdf::CG_Block> block) {
        // Insert the block.
        cg.insert(block);

        // Relink the chain.
        auto iter = cg.begin();
        auto current = *iter;
        iter++;

        if(current) {
            setFirstCG(current);
            current->setNextCG(std::shared_ptr<CG_Block>());
        }

        while(iter != cg.end()) {
            // Set this to point to the next node.
            current->setNextCG(*iter);

            // Clear the entry for the next node.
            current = *iter;
            current->setNextCG(std::shared_ptr<CG_Block>());

            // Go to next node.
            iter++;
        }

        if(cg.size() == 1) {
            current->setNextCG(std::shared_ptr<CG_Block>());
        }
    }

    void DG_Block::removeCG(std::shared_ptr<mdf::CG_Block> block) {
        // Remove the block.
        cg.erase(block);

        // Relink the chain.
        auto iter = cg.begin();
        auto current = *iter;
        iter++;

        if(current) {
            setFirstCG(current);
            current->setNextCG(std::shared_ptr<CG_Block>());
        }

        while(iter != cg.end()) {
            // Set this to point to the next node.
            current->setNextCG(*iter);

            // Clear the entry for the next node.
            current = *iter;
            current->setNextCG(std::shared_ptr<CG_Block>());

            // Go to next node.
            iter++;
        }

        if(cg.size() == 1) {
            current->setNextCG(std::shared_ptr<CG_Block>());
        }
    }

    //endregion

}
