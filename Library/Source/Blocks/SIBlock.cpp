#include "SIBlock.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

  SIBlockBusType operator&(SIBlockBusType const &lhs, SIBlockBusType const &rhs) {
    using ut = std::underlying_type_t<SIBlockBusType>;

    ut intermediate = static_cast<ut>(lhs) & static_cast<ut>(rhs);

    return static_cast<SIBlockBusType>(intermediate);
  }

#pragma pack(push, 1)
  struct SIBlockData {
    boost::endian::little_uint8_buf_t type;
    boost::endian::little_uint8_buf_t bus_type;
    boost::endian::little_uint8_buf_t flags;
    boost::endian::little_uint8_buf_t reserved[5];
  };
#pragma pack(pop)

  SIBlockBusType SIBlock::getBusType() const {
    return static_cast<SIBlockBusType>(busType);
  }

  std::shared_ptr<MDBlock> SIBlock::getComment() const {
    return std::dynamic_pointer_cast<MDBlock>(links[2]);
  }

  bool SIBlock::load(uint8_t const *dataPtr) {
    bool result = false;

    // Load data into a struct for easier access.
    auto ptr = reinterpret_cast<SIBlockData const *>(dataPtr);
    type = ptr->type.value();
    busType = ptr->bus_type.value();
    flags = ptr->flags.value();

    result = true;

    return result;
  }

  bool SIBlock::saveBlockData(uint8_t *dataPtr) {
    auto ptr = reinterpret_cast<SIBlockData *>(dataPtr);

    ptr->type = type;
    ptr->bus_type = busType;
    ptr->flags = flags;

    return true;
  }

}
