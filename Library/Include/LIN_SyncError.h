#ifndef MDFSIMPLECONVERTERS_LIN_SYNCERROR_H
#define MDFSIMPLECONVERTERS_LIN_SYNCERROR_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct LIN_SyncError {
        // From general MDF properties.
        std::chrono::nanoseconds TimeStamp;

        // From general event properties.
        uint8_t BusChannel;

        // LIN logging specific.
        float Baudrate;
    };

    std::ostream &operator<<(std::ostream &stream, LIN_SyncError const &record);

}

#endif //MDFSIMPLECONVERTERS_LIN_SYNCERROR_H
