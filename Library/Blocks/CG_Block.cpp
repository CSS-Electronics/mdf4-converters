#include "CG_Block.h"

#include "DG_Block.h"

namespace mdf {

    enum CG_LinkDefinitions : int {
        CG_LinkAcquisitionName = 2,
        CG_LinkAcquisitionSource = 3,
        CG_LinkComment = 5,
        CG_LinkFirstChannel = 1,
        CG_LinkNextCG = 0,
        CG_LinkSR = 4,
    };

    static uint64_t constexpr size = 52;
    static uint64_t constexpr linkCount = 6;

    //region Constructors

    CG_Block::CG_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_DG;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;

        // Others.
        bytesUsed = 0;
    }

    CG_Block::CG_Block(CG_Block const& value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy.
        bytesUsed = value.bytesUsed;

        // Create copies of the links.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);
        setAcquisitionName(createCopyIfSet(value.getAcquisitionName()));
        setAcquisitionSource(createCopyIfSet(value.getAcquisitionSource()));
        setComment(createCopyIfSet(value.getComment()));
        setFirstCN(createCopyIfSet(value.getFirstCN()));
        setNextCG(createCopyIfSet(value.getNextCG()));

        // Re-link.
        postLoad();
    }

    //endregion

    //region Inherited functions

    bool CG_Block::load(std::istream &stream) {
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

    void CG_Block::postLoad() {
        // If this is a VLSD channel, no children are present.
        if(isVLSD()) {
            bytesUsed = -1;
        } else {
            // Walk all CN blocks and register them in the set. Read size information if at the same time.
            uint64_t highestValue = 0;

            std::shared_ptr<CN_Block> cnBlock = getFirstCN();
            while (cnBlock) {
                uint64_t value =
                    cnBlock->data.cn_byte_offset * 8 + cnBlock->data.cn_bit_offset + cnBlock->data.cn_bit_count;

                cn.insert(cnBlock);
                cnBlock = cnBlock->getNextCN();

                if (value > highestValue) {
                    highestValue = value;
                }
            }

            // Scale to bytes.
            bytesUsed = highestValue / 8;

            if (highestValue % 8 != 0) {
                bytesUsed += 1;
            }
        }
    }

    void CG_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        links.cg_next = setLinkIfValidPointer(getNextCG());
        links.cn_first = setLinkIfValidPointer(getFirstCN());
        links.tx_acq_name = setLinkIfValidPointer(getAcquisitionName());
        links.si_acq_source = setLinkIfValidPointer(getAcquisitionSource());
        links.sr_first = 0;
        links.md_comment = setLinkIfValidPointer(getComment());

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    //endregion

    //Region Getters

    std::shared_ptr<TX_Block> CG_Block::getAcquisitionName() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[CG_LinkAcquisitionName]);
    }

    std::shared_ptr<SI_Block> CG_Block::getAcquisitionSource() const {
        return std::dynamic_pointer_cast<SI_Block>(blockLinks[CG_LinkAcquisitionSource]);
    }

    std::shared_ptr<MD_Block> CG_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[CG_LinkComment]);
    }

    std::shared_ptr<CN_Block> CG_Block::getFirstCN() const {
        return std::dynamic_pointer_cast<CN_Block>(blockLinks[CG_LinkFirstChannel]);
    }

    std::shared_ptr<CG_Block> CG_Block::getNextCG() const {
        return std::dynamic_pointer_cast<CG_Block>(blockLinks[CG_LinkNextCG]);
    }

    //endregion

    //region Setters

    void CG_Block::setAcquisitionName(std::shared_ptr<mdf::TX_Block> const& block) {
        blockLinks[CG_LinkAcquisitionName] = block;
    }

    void CG_Block::setAcquisitionSource(std::shared_ptr<mdf::SI_Block> const& block) {
        blockLinks[CG_LinkAcquisitionSource] = block;
    }

    void CG_Block::setComment(std::shared_ptr<mdf::MD_Block> const& block) {
        blockLinks[CG_LinkComment] = block;
    }

    void CG_Block::setFirstCN(std::shared_ptr<mdf::CN_Block> const& block) {
        blockLinks[CG_LinkFirstChannel] = block;
    }

    void CG_Block::setNextCG(std::shared_ptr<mdf::CG_Block> const& block) {
        blockLinks[CG_LinkNextCG] = block;
    }

    //endregion

    //region Others

    void CG_Block::addCN(std::shared_ptr<mdf::CN_Block> const &block) {
        // Insert the block.
        cn.insert(block);

        // Relink the chain.
        auto iter = cn.begin();
        auto current = *iter;
        iter++;

        while(iter != cn.end()) {
            current->setNextCN(*iter);
            current = *iter;
            iter++;
        }
    }

    bool CG_Block::isVLSD() const {
        return data.flags & CG_FlagVLSD;
    }

    //endregion

}
