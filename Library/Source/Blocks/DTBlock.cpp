#include "DTBlock.h"

namespace mdf {

    constexpr MdfHeader DTBlockHeader = {
        .blockType = MdfBlockType_DT,
        .blockSize = 24,
        .linkCount = 0
    };

    DTBlock::DTBlock() {
        header = DTBlockHeader;
        dataPtr = nullptr;
    }

    bool DTBlock::load(uint8_t const* dataPtr_) {
        bool result = false;
        this->dataPtr = dataPtr_;

        result = true;

        return result;
    }

}
