#include <algorithm>

#include "SDBlockContinuous.h"

namespace mdf {

  uint8_t const *SDBlockContinuous::operator[](std::size_t index) {
    // Since this is a continuous block, simply use the data pointer and an offset.
    uint8_t const* result = dataPtr + index;

    return result;
  }

  bool SDBlockContinuous::saveBlockData(uint8_t *outputDataPtr) {
    // Get the total size of the block, without the block header.
    std::size_t totalSize = header.blockSize - sizeof(MdfHeader);

    // Copy all bytes from the input to the output in a continuous fashion.
    std::copy_n(this->dataPtr, totalSize, outputDataPtr);

    return true;
  }

  bool SDBlockContinuous::load(uint8_t const *inputDataPtr) {
    // Save the data pointer for easier access.
    dataPtr = inputDataPtr;

    return true;
  }

}