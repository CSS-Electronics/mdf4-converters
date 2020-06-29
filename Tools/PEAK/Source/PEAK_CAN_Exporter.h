#ifndef TOOLS_PEAK_CAN_EXPORTER_H
#define TOOLS_PEAK_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <chrono>
#include <ctime>
#include <sstream>

#include "CAN_DataFrame.h"
#include "FileInfo.h"
#include "PEAK_TraceFormat.h"

namespace mdf::tools::peak {

    // Define new floating precision time base for time in ms.
    using milliseconds = std::chrono::duration<double, std::milli>;

    class PEAK_CAN_Exporter : public tools::shared::GenericRecordExporter<PEAK_Record> {
    public:
        explicit PEAK_CAN_Exporter(std::ostream &output, FileInfo const &fileInfo);

        virtual void correctHeader() = 0;

    protected:
        milliseconds convertTimestampToRelative(std::chrono::nanoseconds timeStamp) const;

        FileInfo const &fileInfo;

        unsigned long long recordCounter;

        std::fpos<mbstate_t> startTimePosition;
        std::chrono::nanoseconds headerTimeStamp;
        bool timeStampSet = false;
    };
    
    template<typename T> inline constexpr bool always_false_v = false;

}

#endif //TOOLS_PCAP_CAN_EXPORTER_H
