#ifndef MDFSORTER_MD_BLOCK_H
#define MDFSORTER_MD_BLOCK_H

#include "MDF_Block.h"

#include <string>

namespace mdf {

    struct MD_Block : public MDF_Block {
        // Constructors.
        MD_Block();
        explicit MD_Block(std::string const& value);
        MD_Block(MD_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        // Setters.
        std::string getData() const;

    private:
        std::string data;
    };

}

#endif //MDFSORTER_MD_BLOCK_H
