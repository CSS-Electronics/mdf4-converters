#include "DTBlockRaw.h"

#include <stdexcept>

namespace mdf {

    bool DTBlockRaw::saveBlockData(std::streambuf *dataPtr) {
        throw std::runtime_error("This method should never be called");
    }

    std::size_t DTBlockRaw::operator[](std::size_t index) {
        return 0;
    }

}
