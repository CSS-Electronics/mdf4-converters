#ifndef MDFSIMPLECONVERTERS_LIN_TRANSMISSIONEROR_H
#define MDFSIMPLECONVERTERS_LIN_TRANSMISSIONEROR_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct LIN_TransmissionError {
        // From general MDF properties.
        std::chrono::nanoseconds TimeStamp;

        // From general event properties.
        uint8_t BusChannel;

        // LIN logging specific.
        uint8_t ID;
    };

    std::ostream &operator<<(std::ostream &stream, LIN_TransmissionError const &record);

}

#endif //MDFSIMPLECONVERTERS_LIN_TRANSMISSIONEROR_H
