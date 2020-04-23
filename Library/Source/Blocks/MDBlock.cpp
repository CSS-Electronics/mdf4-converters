#include "MDBlock.h"

#include <sstream>

namespace mdf {

  std::string_view MDBlock::getMetaData() const {
    return std::string_view(metaData);
  }

  bool MDBlock::load(uint8_t const *dataPtr) {
    bool result = false;

    // Load the data as string.
    std::stringstream ss;
    ss.write(reinterpret_cast<char const *>(dataPtr), header.blockSize - sizeof(header));
    metaData = ss.str();

    result = true;

    return result;
  }

  bool MDBlock::saveBlockData(uint8_t *dataPtr) {
    std::copy(std::begin(metaData), std::end(metaData), dataPtr);

    return true;
  }

}
