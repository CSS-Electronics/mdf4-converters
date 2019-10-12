#ifndef MDFSORTER_CC_BLOCK_H
#define MDFSORTER_CC_BLOCK_H

#include "MDF_Block.h"

#include "MD_Block.h"
#include "TX_Block.h"

namespace mdf {

    struct CC_Links {
        uint64_t tx_name;
        uint64_t md_unit;
        uint64_t md_comment;
        uint64_t cc_inverse;
    };

    struct CC_Data {
        uint8_t cc_type;
        uint8_t cc_precision;
        uint16_t ch_flags;
        uint16_t cc_ref_count;
        uint16_t cc_val_count;
        double cc_phy_range_min;
        double cc_phy_range_max;
    };

    struct CC_Block : public MDF_Block {
        // Constructors.
        CC_Block();
        CC_Block(CC_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        std::shared_ptr<TX_Block> getName() const;
        std::shared_ptr<MDF_Block> getUnitGeneric() const;
        std::shared_ptr<MD_Block> getUnitMD() const;
        std::shared_ptr<TX_Block> getUnitTX() const;
        MDF_Type getUnitType() const;
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<CC_Block> getInverse() const;

        // Setters.
        void setName(std::shared_ptr<TX_Block> const& value);
        void setUnit(std::shared_ptr<MDF_Block> const& value);
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setInverse(std::shared_ptr<CC_Block> const& value);
    private:
        CC_Links links;
        CC_Data data;
        std::vector<double> values;
    };

}

#endif //MDFSORTER_CC_BLOCK_H
