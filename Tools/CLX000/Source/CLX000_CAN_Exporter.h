#ifndef TOOLS_CLX_CAN_EXPORTER_H
#define TOOLS_CLX_CAN_EXPORTER_H

#include <ctime>
#include <sstream>
#include <string_view>

#include "CLX000_Configuration.h"
#include "CommonOptions.h"
#include "GenericRecordExporter.h"

#include "CANRecord.h"
#include "FileInfo.h"

namespace ts = mdf::tools::shared;

namespace mdf::tools::clx {

    class CLX000_CAN_Exporter : public ts::GenericRecordExporter<CANRecord> {
    public:
        explicit CLX000_CAN_Exporter(
                std::ostream& output,
                FileInfo const& fileInfo,
                uint8_t busChannel,
                CLX000_Configuration const& configuration,
                shared::DisplayTimeFormat displayLocalTime
            );
        void writeHeader() override;
        void writeRecord(CANRecord const& record) override;
        void correctHeader();
    private:
        void convertTimestampWithYear(std::time_t const& timeStamp, std::ostream &output);

        void convertTimestampToFormat(std::chrono::nanoseconds const& timeStamp, std::ostream& output);

        uint8_t const busChannel;
        std::string_view const delimiter;
        FileInfo const& fileInfo;

        std::fpos<mbstate_t> datePosition;
        time_t headerTimeStamp;
        bool timeStampSet = false;

        CLX000_Configuration const& configuration;
        std::stringstream timeStampStream;
        tools::shared::DisplayTimeFormat displayLocalTime;
    };

}

#endif //TOOLS_CLX_CAN_EXPORTER_H
