#ifndef MDFSIMPLECONVERTERS_CANRECORD_H
#define MDFSIMPLECONVERTERS_CANRECORD_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

  struct CANRecord {
    // From general MDF properties.
    std::chrono::nanoseconds TimeStamp;

    // From general event properties.
    uint8_t BusChannel;

    // CAN logging specific.
    uint32_t ID;
    bool IDE;
    uint8_t DLC;
    uint8_t DataLength;
    bool Dir;
    bool EDL;
    bool BRS;
    boost::container::static_vector<uint8_t, 64> DataBytes;
  };

  std::ostream &operator<<(std::ostream & stream, CANRecord const& record);

}

#endif //MDFSIMPLECONVERTERS_CANRECORD_H
