#ifndef MDFSORTER_FH_BLOCK_H
#define MDFSORTER_FH_BLOCK_H

#include "MDF_Block.h"

#include "MD_Block.h"

namespace mdf {

    struct FH_Links {
        uint64_t fh_next;
        uint64_t md_comment;
    };

    struct FH_Data {
        uint64_t time_ns;
        int16_t tz_offset_min;
        int16_t dst_offset_min;
        uint8_t time_flags;
        uint8_t reserved[3];
    };

    struct FH_Block : public MDF_Block {
        // Constructors.
        FH_Block();
        FH_Block(FH_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<FH_Block> getNextFH() const;

        // Setters.
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setNextFH(std::shared_ptr<FH_Block> const& value);

        FH_Links links;
        FH_Data data;


    private:
        std::shared_ptr<MD_Block> comment;
    };

}

#endif //MDFSORTER_FH_BLOCK_H
