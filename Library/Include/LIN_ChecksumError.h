#ifndef MDFSIMPLECONVERTERS_LIN_CHECKSUMERROR_H
#define MDFSIMPLECONVERTERS_LIN_CHECKSUMERROR_H

#include <chrono>
#include <cstdint>
#include <ostream>

#include <boost/container/static_vector.hpp>

namespace mdf {

    struct LIN_ChecksumError {
        // From general MDF properties.
        std::chrono::nanoseconds TimeStamp;

        // From general event properties.
        uint8_t BusChannel;

        // LIN logging specific.
        uint8_t ID;
        bool Dir;
        uint8_t ReceivedDataByteCount;
        uint8_t DataLength;
        uint8_t Checksum;
        boost::container::static_vector<uint8_t, 8> DataBytes;
    };

    std::ostream &operator<<(std::ostream &stream, LIN_ChecksumError const &record);

}

#endif //MDFSIMPLECONVERTERS_LIN_CHECKSUMERROR_H
