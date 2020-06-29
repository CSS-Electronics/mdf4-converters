#ifndef MDFSIMPLECONVERTERS_LIN_RECEIVEERROR_H
#define MDFSIMPLECONVERTERS_LIN_RECEIVEERROR_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct LIN_ReceiveError {
        // From general MDF properties.
        std::chrono::nanoseconds TimeStamp;

        // From general event properties.
        uint8_t BusChannel;

        // LIN logging specific.
        uint8_t ID;
    };

    std::ostream &operator<<(std::ostream &stream, LIN_ReceiveError const &record);

}

#endif //MDFSIMPLECONVERTERS_LIN_RECEIVEERROR_H
