#include "DTBlockRaw.h"

#include <stdexcept>

namespace mdf {

    bool DTBlockRaw::saveBlockData(uint8_t* dataPtr) {
        throw std::runtime_error("This method should never be called");
    }

  uint64_t DTBlockRaw::operator[](std::size_t index) {
    return 0;
  }

}
