#include <algorithm>

#include "DTBlockSingleContinuous.h"

namespace mdf {

  DTBlockSingleContinuous::DTBlockSingleContinuous(const mdf::DTBlock &parent, std::size_t recordSize) :
    recordSize(recordSize),
    DTBlock(parent)
    {
    //
  }

  uint8_t const *DTBlockSingleContinuous::operator[](std::size_t index) {
    // Since this is a continuous block, the n'th record is simple an offset into the data block.
    uint8_t const* result = dataPtr + index * recordSize;

    return result;
  }

  bool DTBlockSingleContinuous::saveBlockData(uint8_t* dataPtr) {
    // Since only a single type of record is present, simply write all the records.
    std::copy_n(this->dataPtr, header.blockSize, dataPtr);

    return true;
  }

}
