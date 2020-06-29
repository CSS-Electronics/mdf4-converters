#ifndef TOOLS_ASC_CAN_EXPORTER_H
#define TOOLS_ASC_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "CANRecord.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::asc {

  class ASC_CAN_Exporter : public tools::shared::GenericRecordExporter<CANRecord> {
  public:
    explicit ASC_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
      tools::shared::DisplayTimeFormat displayLocalTime);

    void writeHeader() override;

    void writeRecord(CANRecord const &record) override;

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
  };

}

#endif //TOOLS_ASC_CAN_EXPORTER_H
