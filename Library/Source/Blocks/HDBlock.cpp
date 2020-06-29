#include "HDBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

#pragma pack(push, 1)
    struct HDBlockData {
        be::little_uint64_buf_t start_time_ns;
        be::little_int16_buf_t tz_offset_min;
        be::little_int16_buf_t dst_offset_min;
        be::little_uint8_buf_t time_flags;
        be::little_uint8_buf_t time_class;
        be::little_uint8_buf_t flags;
        be::little_uint8_buf_t reserved;
        be::little_float64_buf_t start_angle_rad;
        be::little_float64_buf_t start_distance_m;
    };
#pragma pack(pop)

    bool HDBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        HDBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char *>(&rawData), sizeof(rawData));

        if (bytesRead != sizeof(rawData)) {
            return false;
        }

        startTimeNs = rawData.start_time_ns.value();
        tzOffsetMin = rawData.tz_offset_min.value();
        dstOffsetMin = rawData.dst_offset_min.value();
        timeFlags = rawData.time_flags.value();
        timeClass = rawData.time_class.value();
        flags = rawData.flags.value();
        start_angle_rad = rawData.start_angle_rad.value();
        start_distance_m = rawData.start_distance_m.value();

        result = true;

        return result;
    }

    std::shared_ptr<DGBlock> HDBlock::getFirstDGBlock() const {
        return std::dynamic_pointer_cast<DGBlock>(links[0]);
    }

    std::shared_ptr<MDBlock> HDBlock::getComment() const {
        return std::dynamic_pointer_cast<MDBlock>(links[5]);
    }

    bool HDBlock::saveBlockData(uint8_t *dataPtr) {
        auto ptr = reinterpret_cast<HDBlockData *>(dataPtr);
        ptr->start_time_ns = startTimeNs;
        ptr->tz_offset_min = tzOffsetMin;
        ptr->dst_offset_min = dstOffsetMin;
        ptr->time_flags = timeFlags;
        ptr->time_class = timeClass;
        ptr->flags = flags;
        ptr->start_angle_rad = start_angle_rad;
        ptr->start_distance_m = start_distance_m;

        return true;
    }

    uint64_t HDBlock::getStartTimeNs() const {
        return startTimeNs;
    }

    int16_t HDBlock::getTzOffsetMin() const {
        return tzOffsetMin;
    }

    int16_t HDBlock::getDstOffsetMin() const {
        return dstOffsetMin;
    }


}
