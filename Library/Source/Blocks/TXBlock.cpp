#include "TXBlock.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <streambuf>

namespace mdf {

    bool TXBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load the data as string.
        std::stringstream ss;
        std::copy_n(
                std::istreambuf_iterator<char>(stream.get()),
                header.blockSize - sizeof(header),
                std::ostream_iterator<char>(ss)
        );
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
