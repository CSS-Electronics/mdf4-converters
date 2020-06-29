#include <algorithm>

#include "DTBlockSingleContinuous.h"

namespace mdf {

    DTBlockSingleContinuous::DTBlockSingleContinuous(const mdf::DTBlock &parent, std::size_t recordSize) :
            recordSize(recordSize),
            DTBlock(parent) {
        //
    }

    uint64_t DTBlockSingleContinuous::operator[](std::size_t index) {
        // Since this is a continuous block, the n'th record is simple an offset into the data block.
        uint64_t location = getFileLocation();

        location += index * recordSize;

        return location;
    }

    bool DTBlockSingleContinuous::saveBlockData(uint8_t *dataPtr) {
        // Since only a single type of record is present, simply write all the records.
        // TODO: FIX
        //std::copy_n(this->dataPtr, header.blockSize, dataPtr);

        return true;
    }

}
