#include "DTBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    constexpr MdfHeader DTBlockHeader(MdfBlockType_DT, 24, 0);

    DTBlock::DTBlock() {
        header = DTBlockHeader;
    }

    bool DTBlock::load(std::shared_ptr<std::streambuf> stream_) {
        bool result = false;
        stream = stream_;
        rawFileLocation = stream->pubseekoff(0, std::ios_base::cur) - std::streamoff(24);

        result = true;

        return result;
    }

}
