#ifndef MDFSORTER_CN_BLOCK_H
#define MDFSORTER_CN_BLOCK_H

#include "MDF_Block.h"

namespace mdf {

    enum CN_Type : uint8_t {
        CN_TypeValue = 0x00,
        CN_TypeVLSD = 0x01
    };

    struct CN_Links {
        uint64_t cn_next;
        uint64_t composition;
        uint64_t tx_name;
        uint64_t si_source;
        uint64_t cc_conversion;
        uint64_t data;
        uint64_t md_unit;
        uint64_t md_comment;
    };

    struct CN_Data {
        uint8_t cn_type;
        uint8_t cn_sync_type;
        uint8_t cn_data_type;
        uint8_t cn_bit_offset;
        uint32_t cn_byte_offset;
        uint32_t cn_bit_count;
        uint32_t cn_flags;
        uint32_t cn_inval_bit_pos;
        uint8_t cn_precision;
        uint8_t reserved;
        uint16_t cn_attachment_count;
        double cn_val_range_min;
        double cn_val_range_max;
        double cn_limit_min;
        double cn_limit_max;
        double cn_limit_ext_min;
        double cn_limit_ext_max;
    };

    struct CN_Block : public MDF_Block {
        // Constructors.
        CN_Block();
        CN_Block(CN_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void postLoad() override;
        void save(std::ostream& stream) override;

        // Getters.
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<CN_Block> getComposition() const;
        std::shared_ptr<CC_Block> getConversion() const;
        std::shared_ptr<MDF_Block> getDataGeneric() const;
        std::shared_ptr<CG_Block> getDataCG() const;
        std::shared_ptr<SD_Block> getDataSD() const;
        MDF_Type getDataType() const;
        std::shared_ptr<TX_Block> getName() const;
        std::shared_ptr<CN_Block> getNextCN() const;
        std::shared_ptr<SI_Block> getSource() const;
        std::shared_ptr<MD_Block> getUnit() const;

        // Setters
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setComposition(std::shared_ptr<CN_Block> const& value);
        void setConversion(std::shared_ptr<CC_Block> const& value);
        void setData(std::shared_ptr<MDF_Block> const& value);
        void setName(std::shared_ptr<TX_Block> const& value);
        void setNextCN(std::shared_ptr<CN_Block> const& value);
        void setSource(std::shared_ptr<SI_Block> const& value);
        void setUnit(std::shared_ptr<MD_Block> const& value);

        // Others.
        bool isComposite() const;
        bool isVLSD() const;

        CN_Links links;
        CN_Data data;

        std::vector<std::shared_ptr<CN_Block>> composite;
    private:
    };

}

#endif //MDFSORTER_CN_BLOCK_H
