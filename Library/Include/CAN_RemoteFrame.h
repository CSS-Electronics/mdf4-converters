#ifndef MDFSIMPLECONVERTERS_CAN_REMOTEFRAME_H
#define MDFSIMPLECONVERTERS_CAN_REMOTEFRAME_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct CAN_RemoteFrame {
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
    };

    std::ostream &operator<<(std::ostream &stream, CAN_RemoteFrame const &record);

}

#endif //MDFSIMPLECONVERTERS_CAN_REMOTEFRAME_H
