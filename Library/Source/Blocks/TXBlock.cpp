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

    bool TXBlock::saveBlockData(std::streambuf *stream) {
        std::streamsize written = stream->sputn(text.c_str(), text.size());
        written += stream->sputc('\0');

        return (written == (text.size() + 1));
    }

    std::string_view TXBlock::getText() const {
        return std::string_view(text.c_str());
    }

}
