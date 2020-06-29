#ifndef TOOLS_ASC_LIN_EXPORTER_H
#define TOOLS_ASC_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "LIN_Frame.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::asc {

    class ASC_LIN_Exporter : public tools::shared::GenericRecordExporter<LIN_Frame> {
    public:
        explicit ASC_LIN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                  tools::shared::DisplayTimeFormat displayLocalTime);

        void writeHeader() override;

        void writeRecord(LIN_Frame const &record) override;

    private:
        std::string const delimiter = ";";
        std::chrono::nanoseconds firstTimeStamp;
        bool firstTimeStampSet = false;
        tools::shared::DisplayTimeFormat displayLocalTime;

        std::stringstream timeStampString;
        mdf::tools::shared::ParsedFileInfo const &fileInfo;

        std::string convertTimestamp(double const &timeStamp);

        std::string convertTimestamp(std::time_t const &timeStamp);
    };

}

#endif //TOOLS_ASC_LIN_EXPORTER_H
