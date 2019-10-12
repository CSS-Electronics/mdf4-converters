#ifndef MDFSORTER_CG_BLOCK_H
#define MDFSORTER_CG_BLOCK_H

#include "MDF_Block.h"

#include <set>

#include "CN_Block.h"
#include "MD_Block.h"
#include "SI_Block.h"
#include "TX_Block.h"

namespace mdf {

    enum CG_Flags : uint16_t {
        CG_FlagVLSD = 0x01,
    };

    struct CG_Links {
        uint64_t cg_next;
        uint64_t cn_first;
        uint64_t tx_acq_name;
        uint64_t si_acq_source;
        uint64_t sr_first;
        uint64_t md_comment;
    };

    struct CG_Data {
        uint64_t record_id;
        uint64_t cycle_count;
        uint16_t flags;
        uint16_t path_separator;
        uint8_t reserved[4];
        uint32_t data_bytes;
        uint32_t inval_bytes;
    };

    struct CG_Block : public MDF_Block {

        // Constructors.
        CG_Block();
        CG_Block(CG_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void postLoad() override;
        void save(std::ostream& stream) override;

        // Getters.
        std::shared_ptr<TX_Block> getAcquisitionName() const;
        std::shared_ptr<SI_Block> getAcquisitionSource() const;
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<CN_Block> getFirstCN() const;
        std::shared_ptr<CG_Block> getNextCG() const;

        // Setters.
        void setAcquisitionName(std::shared_ptr<TX_Block> const& block);
        void setAcquisitionSource(std::shared_ptr<SI_Block> const& block);
        void setComment(std::shared_ptr<MD_Block> const& block);
        void setFirstCN(std::shared_ptr<CN_Block> const& block);
        void setNextCG(std::shared_ptr<CG_Block> const& block);

        CG_Links links;
        CG_Data data;

        void addCN(std::shared_ptr<mdf::CN_Block> const& block);

        std::set<std::shared_ptr<CN_Block>> cn;
        bool isVLSD() const;

        int64_t bytesUsed;
    private:
    };

}

#endif //MDFSORTER_CG_BLOCK_H
