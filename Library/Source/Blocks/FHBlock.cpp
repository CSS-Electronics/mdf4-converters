#include "FHBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    #pragma pack(push, 1)
    struct FHBlockData {
        be::little_uint64_buf_t time_ns;
        be::little_uint16_buf_t tz_offset_min;
        be::little_uint16_buf_t dst_offset_min;
        be::little_uint8_buf_t time_flags;
        be::little_uint8_buf_t reserved[3];
    };
    #pragma pack(pop)

    bool FHBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        FHBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char*>(&rawData), sizeof(rawData));

        if(bytesRead != sizeof(rawData)) {
            return false;
        }

        timeNs = rawData.time_ns.value();
        tzOffsetMin = rawData.tz_offset_min.value();
        dstOffsetMin = rawData.dst_offset_min.value();
        timeFlags = rawData.time_flags.value();

        result = true;

        return result;
    }

    bool FHBlock::saveBlockData(uint8_t* dataPtr) {
        auto ptr = reinterpret_cast<FHBlockData*>(dataPtr);
        ptr->time_ns = timeNs;
        ptr->tz_offset_min = tzOffsetMin;
        ptr->dst_offset_min = dstOffsetMin;
        ptr->time_flags = timeFlags;

        return true;
    }

}
