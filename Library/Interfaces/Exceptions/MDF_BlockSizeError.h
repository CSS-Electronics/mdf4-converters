#ifndef MDFSORTER_MDF_BLOCKSIZEERROR_H
#define MDFSORTER_MDF_BLOCKSIZEERROR_H

#include "MDF_Exception.h"

#include "MDF_Block.h"

namespace mdf {

    struct MDF_BlockSizeError : public MDF_Exception {
        static MDF_BlockSizeError Create(std::shared_ptr<MDF_Block> block, uint64_t finalLocation);

        explicit MDF_BlockSizeError(std::string const& what);
    };

}

#endif //MDFSORTER_MDF_BLOCKSIZEERROR_H
