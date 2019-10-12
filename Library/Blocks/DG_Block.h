#ifndef MDFSORTER_DG_BLOCK_H
#define MDFSORTER_DG_BLOCK_H

#include "MDF_Block.h"

#include <set>

#include "CG_Block.h"
#include "DT_Block.h"

namespace mdf {

    struct DG_Links {
        uint64_t dg_next;
        uint64_t cg_first;
        uint64_t data;
        uint64_t md_comment;
    };

    struct DG_Data {
        uint8_t dg_rec_id_size;
        uint8_t reserved[7];
    };

    struct DG_Block : public MDF_Block {
        // Constructors.
        DG_Block();
        DG_Block(DG_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;
        void postLoad() override;

        // Getters.
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<DT_Block> getDataBlock() const;
        std::shared_ptr<CG_Block> getFirstCG() const;
        std::shared_ptr<DG_Block> getNextDG() const;

        // Setters.
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setDataBlock(std::shared_ptr<DT_Block> const& value);
        void setFirstCG(std::shared_ptr<CG_Block> const& value);
        void setNextDG(std::shared_ptr<DG_Block> const& value);

        // Others.
        void addCG(std::shared_ptr<CG_Block> block);
        void removeCG(std::shared_ptr<CG_Block> block);

        std::set<std::shared_ptr<CG_Block>> cg;
        void convertCGVLtoSD();

        std::shared_ptr<CN_Block> findVLSDChannel(std::shared_ptr<CG_Block> const& block) const;

        DG_Links links;
        DG_Data data;
    private:
    };

}

#endif //MDFSORTER_DG_BLOCK_H
