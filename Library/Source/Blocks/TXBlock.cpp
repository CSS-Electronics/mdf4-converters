#include "TXBlock.h"

#include <sstream>

namespace mdf {

    bool TXBlock::load(uint8_t const* dataPtr) {
        bool result = false;

        // Load the data as string.
        std::stringstream ss;
        ss.write(reinterpret_cast<char const*>(dataPtr), header.blockSize - sizeof(header));
        text = ss.str();

        result = true;

        return result;
    }

    bool TXBlock::saveBlockData(uint8_t* dataPtr) {
        std::copy(std::begin(text), std::end(text), dataPtr);

        return true;
    }

    std::string_view TXBlock::getText() const {
        return std::string_view(text.c_str());
    }

}
