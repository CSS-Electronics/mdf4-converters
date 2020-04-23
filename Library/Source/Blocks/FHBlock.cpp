#include "FHBlock.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

    #pragma pack(push, 1)
    struct FHBlockData {
        boost::endian::little_uint64_buf_t time_ns;
        boost::endian::little_uint16_buf_t tz_offset_min;
        boost::endian::little_uint16_buf_t dst_offset_min;
        boost::endian::little_uint8_buf_t time_flags;
        boost::endian::little_uint8_buf_t reserved[3];
    };
    #pragma pack(pop)

    bool FHBlock::load(uint8_t const* dataPtr) {
        bool result = false;

        // Load data into a struct for easier access.
        auto ptr = reinterpret_cast<FHBlockData const*>(dataPtr);

        timeNs = ptr->time_ns.value();
        tzOffsetMin = ptr->tz_offset_min.value();
        dstOffsetMin = ptr->dst_offset_min.value();
        timeFlags = ptr->time_flags.value();

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
