#ifndef MDFSORTER_HD_BLOCK_H
#define MDFSORTER_HD_BLOCK_H

#include "MDF_Block.h"

#include <memory>
#include <set>

#include "DG_Block.h"
#include "FH_Block.h"

namespace mdf {

    struct HD_Links {
        // Link section.
        uint64_t dg_first;
        uint64_t fh_first;
        uint64_t ch_first;
        uint64_t at_first;
        uint64_t ev_first;
        uint64_t md_comment;
    };

    struct HD_Data {
        // Data section.
        uint64_t start_time_ns;
        int16_t tz_offset_min;
        int16_t dst_offset_min;
        uint8_t time_flags;
        uint8_t time_class;
        uint8_t flags;
        uint8_t reserved;
        double start_angle_rad;
        double start_distance_m;
    };

    struct HD_Block : public MDF_Block {
        // Constructors.
        HD_Block();
        HD_Block(HD_Block const& value);
        
        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;
        void postLoad() override;

        // Getters.
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<DG_Block> getFirstDG() const;
        std::shared_ptr<FH_Block> getFirstFH() const;

        uint64_t getStartTime() const;
        int16_t getTimezoneOffset() const;

        // Setters.
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setFirstDG(std::shared_ptr<DG_Block> const& value);
        void setFirstFH(std::shared_ptr<FH_Block> const& value);

        // Others.
        void addDG(std::shared_ptr<DG_Block> const& block);
        void addFH(std::shared_ptr<FH_Block> const& block);

        void sortDG();

        std::set<std::shared_ptr<DG_Block>> dg;
        std::set<std::shared_ptr<FH_Block>> fh;

    private:
        HD_Links links;
        HD_Data data;
    };

}

#endif //MDFSORTER_HD_BLOCK_H
