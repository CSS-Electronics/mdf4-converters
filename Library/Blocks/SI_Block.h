#ifndef MDFSORTER_SI_BLOCK_H
#define MDFSORTER_SI_BLOCK_H

#include "MDF_Block.h"

#include "MD_Block.h"
#include "TX_Block.h"

namespace mdf {

    struct SI_Links {
        uint64_t tx_name;
        uint64_t tx_path;
        uint64_t md_comment;
    };

    struct SI_Data {
        uint8_t type;
        uint8_t bus_type;
        uint8_t flags;
        uint8_t reserved[5];
    };

    struct SI_Block : public MDF_Block {
        // Constructors.
        SI_Block();
        SI_Block(SI_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        std::shared_ptr<MD_Block> getComment() const;
        std::shared_ptr<TX_Block> getName() const;
        std::shared_ptr<TX_Block> getPath() const;

        // Setters.
        void setComment(std::shared_ptr<MD_Block> const& value);
        void setName(std::shared_ptr<TX_Block> const& value);
        void setPath(std::shared_ptr<TX_Block> const& value);

    private:
        SI_Links links;
        SI_Data data;
    };

}

#endif //MDFSORTER_SI_BLOCK_H
