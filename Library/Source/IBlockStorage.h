#ifndef MDFSIMPLECONVERTERS_IBLOCKSTORAGE_H
#define MDFSIMPLECONVERTERS_IBLOCKSTORAGE_H

#include <memory>

#include "Blocks/MdfBlock.h"

namespace mdf {

    struct IBlockStorage {
        virtual ~IBlockStorage() = default;
        virtual std::shared_ptr<MdfBlock> getBlockAt(uint64_t address) = 0;
    };

}

#endif //MDFSIMPLECONVERTERS_IBLOCKSTORAGE_H
