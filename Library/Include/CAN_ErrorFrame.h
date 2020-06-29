#ifndef MDFSIMPLECONVERTERS_CAN_ERRORFRAME_H
#define MDFSIMPLECONVERTERS_CAN_ERRORFRAME_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct CAN_ErrorFrame {
        // From general MDF properties.
        std::chrono::nanoseconds TimeStamp;

        // From general event properties.
        uint8_t BusChannel;

        // CAN logging specific.
        uint8_t ErrorType;
    };

    std::ostream &operator<<(std::ostream &stream, CAN_ErrorFrame const &record);

}

#endif //MDFSIMPLECONVERTERS_CAN_ERRORFRAME_H
