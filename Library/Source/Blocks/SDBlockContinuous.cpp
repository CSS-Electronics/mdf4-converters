#include <algorithm>

#include "SDBlockContinuous.h"

namespace mdf {

    std::size_t SDBlockContinuous::operator[](std::size_t index) {
        // Since this is a continuous block, simply use the data pointer and an offset.
        uint64_t location = getFileLocation() + index;
        return location;
    }

    bool SDBlockContinuous::load(std::shared_ptr<std::streambuf> stream_) {
        // Save the data pointer for easier access.
        stream = stream_;

        return true;
    }

    bool SDBlockContinuous::saveBlockData(std::streambuf *outputDataPtr) {
        // Get the total size of the block, without the block header.
        std::size_t totalSize = header.blockSize - sizeof(MdfHeader);

        // Copy all bytes from the input to the output in a continuous fashion.
        // TODO: FIX
        //std::copy_n(this->dataPtr, totalSize, outputDataPtr);

        return true;
    }

}