#include "HDBlock.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

#pragma pack(push, 1)
  struct HDBlockData {
    boost::endian::little_uint64_buf_t start_time_ns;
    boost::endian::little_int16_buf_t tz_offset_min;
    boost::endian::little_int16_buf_t dst_offset_min;
    boost::endian::little_uint8_buf_t time_flags;
    boost::endian::little_uint8_buf_t time_class;
    boost::endian::little_uint8_buf_t flags;
    boost::endian::little_uint8_buf_t reserved;
    boost::endian::little_float64_buf_t start_angle_rad;
    boost::endian::little_float64_buf_t start_distance_m;
  };
#pragma pack(pop)

  bool HDBlock::load(uint8_t const *dataPtr) {
    bool result = false;

    // Load data into a struct for easier access.
    auto ptr = reinterpret_cast<HDBlockData const *>(dataPtr);

    startTimeNs = ptr->start_time_ns.value();
    tzOffsetMin = ptr->tz_offset_min.value();
    dstOffsetMin = ptr->dst_offset_min.value();
    timeFlags = ptr->time_flags.value();
    timeClass = ptr->time_class.value();
    flags = ptr->flags.value();
    start_angle_rad = ptr->start_angle_rad.value();
    start_distance_m = ptr->start_distance_m.value();

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
