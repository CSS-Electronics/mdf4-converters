#ifndef TOOLS_ASC_CAN_EXPORTER_H
#define TOOLS_ASC_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "CAN_DataFrame.h"
#include "CommonOptions.h"
#include "Library.h"

namespace tools::asc {

    class ASC_CAN_Exporter : public tools::shared::GenericRecordExporter<CAN_DataFrame_t> {
    public:
        explicit ASC_CAN_Exporter(std::ostream& output, FileInfo_t const& fileInfo, tools::shared::DisplayTimeFormat displayLocalTime);
        void writeHeader() override;
        void writeRecord(CAN_DataFrame_t record) override;
    private:
        std::string const delimiter = ";";
        uint64_t firstTimeStamp = 0;
        bool firstTimeStampSet = false;
        tools::shared::DisplayTimeFormat displayLocalTime;
        
        std::stringstream timeStampString;
        FileInfo_t const& fileInfo;

        std::string convertTimestamp(double const& timeStamp);
        std::string convertTimestamp(std::time_t const& timeStamp);
    };

}

#endif //TOOLS_ASC_CAN_EXPORTER_H
