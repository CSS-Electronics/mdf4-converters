#include "SDBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

#pragma pack(push, 1)
    struct SDBlockData {
        be::little_uint8_buf_t type;
        be::little_uint8_buf_t bus_type;
        be::little_uint8_buf_t flags;
        be::little_uint8_buf_t reserved[5];
    };
#pragma pack(pop)

    constexpr MdfHeader SDBlockHeader(MdfBlockType_SD, 24, 0);

    SDBlock::SDBlock() {
        header = SDBlockHeader;
    }

}
