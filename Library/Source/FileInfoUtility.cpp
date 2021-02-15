#include "FileInfo.h"

namespace mdf {

    FileInfo::FileInfo() :
        CANMessages(0),
        LINMessages(0),
        Time(0),
        TimezoneOffsetMinutes(0) {
        //
    }

    void FileInfo::clear() {
        CANMessages = 0;
        LINMessages = 0;
        Time = std::chrono::nanoseconds(0);
        TimezoneOffsetMinutes = 0;
    }
}
