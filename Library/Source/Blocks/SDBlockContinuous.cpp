#include <algorithm>

#include "SDBlockContinuous.h"

namespace mdf {

    uint64_t SDBlockContinuous::operator[](std::size_t index) {
        // Since this is a continuous block, simply use the data pointer and an offset.
        uint64_t location = getFileLocation() + index;
        return location;
    }

    bool SDBlockContinuous::saveBlockData(uint8_t *outputDataPtr) {
        // Get the total size of the block, without the block header.
        std::size_t totalSize = header.blockSize - sizeof(MdfHeader);

        // Copy all bytes from the input to the output in a continuous fashion.
        // TODO: FIX
        //std::copy_n(this->dataPtr, totalSize, outputDataPtr);

        return true;
    }

    bool SDBlockContinuous::load(std::shared_ptr<std::streambuf> stream_) {
        // Save the data pointer for easier access.
        stream = stream_;

        return true;
    }

}