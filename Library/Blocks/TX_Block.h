#ifndef MDFSORTER_TX_BLOCK_H
#define MDFSORTER_TX_BLOCK_H

#include "MDF_Block.h"

#include <string>

namespace mdf {
    struct TX_Block : public MDF_Block {
        // Constructors.
        TX_Block();
        TX_Block(TX_Block const& value);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        std::string getText() const;

        // Setters.

    private:
        std::string stringData;
    };
}

#endif //MDFSORTER_TX_BLOCK_H
