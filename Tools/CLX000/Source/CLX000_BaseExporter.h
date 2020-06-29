#ifndef TOOLS_CLX_BASEEXPORTER_H
#define TOOLS_CLX_BASEEXPORTER_H

#include <ctime>

#include "GenericRecordExporter.h"
#include "ParsedFileInfo.h"

#include "CLX000_Configuration.h"
#include "CommonOptions.h"

namespace mdf::tools::clx {

    template<typename T>
    struct CLX000_BaseExporter : public mdf::tools::shared::GenericRecordExporter<T> {
    public:
        CLX000_BaseExporter(
            std::ostream& output,
            mdf::tools::shared::ParsedFileInfo const& fileInfo,
            uint8_t busChannel,
            CLX000_Configuration const& configuration,
            shared::DisplayTimeFormat displayLocalTime
        );
        void writeHeader() override;
        virtual void correctHeader();
    protected:
        void convertTimestampWithYear(std::time_t const& timeStamp, std::ostream &output);

        void convertTimestampToFormat(std::chrono::nanoseconds const& timeStamp, std::ostream& output);

        uint8_t const busChannel;
        std::string_view const delimiter;
        mdf::tools::shared::ParsedFileInfo const& fileInfo;

        std::fpos<mbstate_t> datePosition;
        time_t headerTimeStamp;
        bool timeStampSet = false;

        CLX000_Configuration const& configuration;
        //std::stringstream timeStampStream;
        tools::shared::DisplayTimeFormat displayLocalTime;

        std::optional<uint32_t> bitRate;
    };

}

#endif //TOOLS_CLX_BASEEXPORTER_H
