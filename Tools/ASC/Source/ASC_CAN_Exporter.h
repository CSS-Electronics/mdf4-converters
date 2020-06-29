#ifndef TOOLS_ASC_CAN_EXPORTER_H
#define TOOLS_ASC_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "ASC_Common.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::asc {

    class ASC_CAN_Exporter : public tools::shared::GenericRecordExporter<ASC_Record> {
    public:
        explicit ASC_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                  tools::shared::DisplayTimeFormat displayLocalTime);

        void writeHeader() override;

        void writeRecord(ASC_Record const &record) override;

    private:
        std::string const delimiter = ";";
        std::chrono::nanoseconds firstTimeStamp;
        bool firstTimeStampSet = false;
        tools::shared::DisplayTimeFormat displayLocalTime;

        std::stringstream timeStampString;
        mdf::tools::shared::ParsedFileInfo const &fileInfo;

        std::string convertTimestamp(std::time_t const &timeStamp);

        std::string convertTimestamp(std::chrono::seconds const &timeStamp);

        std::string convertTimestamp(std::chrono::nanoseconds const &timeStamp);

        void write_CAN_DataFrame(mdf::CAN_DataFrame const &record);

        void write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record);
    };

}

#endif //TOOLS_ASC_CAN_EXPORTER_H
