#include "HD_Block.h"

#include <iostream>

namespace mdf {

    enum HD_LinkDefinitions : int {
        HD_LinkAT = 3,
        HD_LinkCH = 2,
        HD_LinkComment = 5,
        HD_LinkEV = 4,
        HD_LinkFirstDG = 0,
        HD_LinkFirstFH = 1,
    };

    static uint64_t constexpr size = sizeof(MDF_Header) + sizeof(HD_Links) + sizeof(HD_Data);
    static uint64_t constexpr linkCount = 6;

    //region Constructors

    HD_Block::HD_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_HD;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to the fixed location.
        fileLocation = 64;
    }

    HD_Block::HD_Block(HD_Block const& value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy.

        // Create copies of the links.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);
        setComment(createCopyIfSet(value.getComment()));
        setFirstDG(createCopyIfSet(value.getFirstDG()));
        setFirstFH(createCopyIfSet(value.getFirstFH()));

        // Re-link.
        postLoad();
    }

    //endregion

    //region Inherited functions

    bool HD_Block::load(std::istream &stream) {
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

    void HD_Block::postLoad() {
        // Walk all DG blocks and register them in the set.
        std::shared_ptr<DG_Block> dgBlock = getFirstDG();
        while(dgBlock) {
            dg.insert(dgBlock);
            dgBlock = dgBlock->getNextDG();
        }

        // Walk all FH blocks and register them in the set.
        std::shared_ptr<FH_Block> fhBlock = getFirstFH();
        while(fhBlock) {
            fh.insert(fhBlock);
            fhBlock = fhBlock->getNextFH();
        }
    }

    void HD_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Update the links.
        links.dg_first = setLinkIfValidPointer(getFirstDG());
        links.fh_first = setLinkIfValidPointer(getFirstFH());
        links.ch_first = 0;
        links.at_first = 0;
        links.ev_first = 0;
        links.md_comment = setLinkIfValidPointer(getComment());

        // Save links.
        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    //endregion

    //region Getters

    std::shared_ptr<MD_Block> HD_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[HD_LinkComment]);
    }

    std::shared_ptr<DG_Block> HD_Block::getFirstDG() const {
        return std::dynamic_pointer_cast<DG_Block>(blockLinks[HD_LinkFirstDG]);
    }

    std::shared_ptr<FH_Block> HD_Block::getFirstFH() const {
        return std::dynamic_pointer_cast<FH_Block>(blockLinks[HD_LinkFirstFH]);
    }

    uint64_t HD_Block::getStartTime() const {
        return data.start_time_ns;
    }

    int16_t HD_Block::getTimezoneOffset() const {
        return data.tz_offset_min;
    }

    //endregion

    //region Setters

    void HD_Block::setComment(const std::shared_ptr<mdf::MD_Block> &value) {
        blockLinks[HD_LinkComment] = value;
    }

    void HD_Block::setFirstDG(const std::shared_ptr<mdf::DG_Block> &value) {
        blockLinks[HD_LinkFirstDG] = value;
    }

    void HD_Block::setFirstFH(const std::shared_ptr<mdf::FH_Block> &value) {
        blockLinks[HD_LinkFirstFH] = value;
    }

    //endregion

    void HD_Block::addDG(std::shared_ptr<mdf::DG_Block> const& block) {
        // Insert the block.
        dg.insert(block);

        // Relink the chain.
        auto iter = dg.begin();
        auto current = *iter;
        iter++;

        while(iter != dg.end()) {
            current->setNextDG(*iter);
            current = *iter;
            iter++;
        }
    }

    void HD_Block::addFH(std::shared_ptr<mdf::FH_Block> const& block) {
        fh.insert(block);
    }

    void HD_Block::sortDG() {
        std::vector<std::shared_ptr<DG_Block>> blocksToAdd;

        // Loop over the DG blocks.
        for(auto const& dgBlock: dg) {
            // Does this contain more than one CG block?
            if(dgBlock->cg.size() > 1) {
                // List of CG blocks to remove from this DG block.
                std::vector<std::shared_ptr<CG_Block>> CGBlocksToRemove;

                // Move each CG after the first to a new DG.
                auto cgIter = dgBlock->cg.begin();

                uint64_t cgRecordId = cgIter->get()->data.record_id;
                uint64_t cgCycleCount = cgIter->get()->data.cycle_count;
                uint64_t cgBytesUsed = cgIter->get()->bytesUsed;

                cgIter++;

                while(cgIter != dgBlock->cg.end()) {
                    // Create a new DG block.
                    std::shared_ptr<DG_Block> createdDGBlock = std::make_shared<DG_Block>();

                    // Copy the settings.
                    createdDGBlock->data = dgBlock->data;
                    std::shared_ptr<CG_Block> cgBlock = *cgIter;
                    cgIter++;

                    // Add the CG block to the new DG block, and remove it from the old.
                    createdDGBlock->addCG(cgBlock);
                    CGBlocksToRemove.push_back(cgBlock);

                    // Add the block to the list.
                    blocksToAdd.push_back(createdDGBlock);

                    // Create a new DT block mapping the relevant data.
                    uint64_t recordID = cgBlock->data.record_id;
                    std::shared_ptr<DT_Block> createdDTBlock = std::make_shared<DT_Block>(*dgBlock->getDataBlock(), recordID);
                    createdDTBlock->header.size = sizeof(MDF_Header) + cgBlock->data.cycle_count * cgBlock->bytesUsed;
                    createdDGBlock->setDataBlock(createdDTBlock);
                }

                // Remove the CG blocks.
                for(auto const& block: CGBlocksToRemove) {
                    dgBlock->removeCG(block);
                }

                // Replace the DT block with a new DT block, with only the relevant data.
                std::shared_ptr<DT_Block> replacementDTBlock = std::make_shared<DT_Block>(*dgBlock->getDataBlock(), cgRecordId);

                // Update the size from the cycle counter.
                replacementDTBlock->header.size += cgCycleCount * cgBytesUsed;

                dgBlock->setDataBlock(replacementDTBlock);
            }
        }

        // Add all the blocks and remove space for the record ID.
        for(auto const& dgBlock: blocksToAdd) {
            addDG(dgBlock);
        }

        for(auto const& dgBlock: dg) {
            // TODO: Figure out why this hack is necessary, since addDG/removeDG should handle it?
            dgBlock->getFirstCG()->setNextCG(std::shared_ptr<CG_Block>());

            dgBlock->data.dg_rec_id_size = 0;
        }
    }

}
