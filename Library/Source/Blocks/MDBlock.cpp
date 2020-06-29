#include "MDBlock.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <streambuf>

namespace mdf {

  std::string_view MDBlock::getMetaData() const {
    return std::string_view(metaData);
  }

  bool MDBlock::load(std::shared_ptr<std::streambuf> stream) {
    bool result = false;

    // Load the data as string.
    std::stringstream ss;
    std::copy_n(
            std::istreambuf_iterator<char>(stream.get()),
            header.blockSize - sizeof(header),
            std::ostream_iterator<char>(ss)
            );

    metaData = ss.str();

    result = true;

    return result;
  }

  bool MDBlock::saveBlockData(uint8_t *dataPtr) {
    std::copy(std::begin(metaData), std::end(metaData), dataPtr);

    return true;
  }

}
