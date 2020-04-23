#ifndef MDFSIMPLECONVERTERS_LINRECORD_H
#define MDFSIMPLECONVERTERS_LINRECORD_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

  struct LINRecord {
    // From general MDF properties.
    std::chrono::nanoseconds TimeStamp;

    // From general event properties.
    uint8_t BusChannel;

    // LIN logging specific.
    uint32_t ID;
    bool Dir;
    uint8_t ReceivedDataByteCount;
    uint8_t DataLength;
    boost::container::static_vector<uint8_t, 8> DataBytes;
  };

  std::ostream &operator<<(std::ostream & stream, LINRecord const& record);

}

#endif //MDFSIMPLECONVERTERS_LINRECORD_H
