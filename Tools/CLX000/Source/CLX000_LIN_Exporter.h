#ifndef TOOLS_CLX000_LIN_EXPORTER_H
#define TOOLS_CLX000_LIN_EXPORTER_H

#include <ctime>
#include <sstream>
#include <string_view>

#include "GenericRecordExporter.h"

#include "CLX000_Configuration.h"
#include "CommonOptions.h"
#include "LINRecord.h"
#include "ParsedFileInfo.h"

namespace ts = mdf::tools::shared;

namespace mdf::tools::clx {

    class CLX000_LIN_Exporter : public ts::GenericRecordExporter<LINRecord> {
    public:
        explicit CLX000_LIN_Exporter(
            std::ostream& output,
            mdf::tools::shared::ParsedFileInfo const& fileInfo,
            uint8_t busChannel,
            CLX000_Configuration const& configuration,
            tools::shared::DisplayTimeFormat displayLocalTime
        );
        void writeHeader() override;
        void writeRecord(LINRecord const& record) override;
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
        mdf::tools::shared::ParsedFileInfo const& fileInfo;

        std::fpos<mbstate_t> datePosition;
        time_t headerTimeStamp;
        bool timeStampSet = false;

        CLX000_Configuration const& configuration;
        std::stringstream timeStampStream;
        tools::shared::DisplayTimeFormat displayLocalTime;
    };

}

#endif //TOOLS_CLX000_LIN_EXPORTER_H
