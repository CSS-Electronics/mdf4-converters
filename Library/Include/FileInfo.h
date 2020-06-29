#ifndef MDFSIMPLECONVERTERS_FILEINFO_H
#define MDFSIMPLECONVERTERS_FILEINFO_H

#include <chrono>
#include <cstdint>

namespace mdf {

    struct FileInfo {
        FileInfo();

        std::chrono::nanoseconds Time;
        int16_t TimezoneOffsetMinutes;
        uint64_t CANMessages;
        uint64_t LINMessages;

        void clear();
    };

}

#endif //MDFSIMPLECONVERTERS_FILEINFO_H
