#include "SDBlock.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

#pragma pack(push, 1)
  struct SDBlockData {
    boost::endian::little_uint8_buf_t type;
    boost::endian::little_uint8_buf_t bus_type;
    boost::endian::little_uint8_buf_t flags;
    boost::endian::little_uint8_buf_t reserved[5];
  };
#pragma pack(pop)

  constexpr MdfHeader SDBlockHeader = {
    .blockType = MdfBlockType_SD,
    .blockSize = 24,
    .linkCount = 0
  };

  SDBlock::SDBlock() {
    header = SDBlockHeader;
  }

}
