#ifndef TOOLS_CLX_CAN_EXPORTER_H
#define TOOLS_CLX_CAN_EXPORTER_H

#include <ctime>
#include <sstream>
#include <string_view>

#include "CLX000_Configuration.h"
#include "CommonOptions.h"
#include "GenericRecordExporter.h"

#include "CAN_DataFrame.h"
#include "Library.h"

namespace ts = tools::shared;

namespace tools::clx {

    class CLX000_CAN_Exporter : public ts::GenericRecordExporter<CAN_DataFrame_t> {
    public:
        explicit CLX000_CAN_Exporter(
                std::ostream& output,
                FileInfo_t const& fileInfo,
                uint8_t busChannel,
                CLX000_Configuration const& configuration,
                tools::shared::DisplayTimeFormat displayLocalTime
            );
        void writeHeader() override;
        void writeRecord(CAN_DataFrame_t record) override;
        void correctHeader();
    private:
        std::string convertTimestampWithYear(std::time_t const& timeStamp);

        std::stringstream timeStream;
        std::string convertTimestampToFormat(std::time_t const& timeStamp);
        void convertTimestampToFormat0(std::time_t const& timeStamp);
        void convertTimestampToFormat1(std::time_t const& timeStamp);
        void convertTimestampToFormat2(std::time_t const& timeStamp);
        void convertTimestampToFormat3(std::time_t const& timeStamp);
        void convertTimestampToFormat4(std::time_t const& timeStamp);
        void convertTimestampToFormat5(std::time_t const& timeStamp);
        void convertTimestampToFormat6(std::time_t const& timeStamp);

        uint8_t const busChannel;
        std::string_view const delimiter;
        FileInfo_t const& fileInfo;

        std::fpos<mbstate_t> datePosition;
        time_t headerTimeStamp;
        bool timeStampSet = false;

        CLX000_Configuration const& configuration;
        std::stringstream timeStampStream;
        tools::shared::DisplayTimeFormat displayLocalTime;
    };

}

#endif //TOOLS_CLX_CAN_EXPORTER_H
