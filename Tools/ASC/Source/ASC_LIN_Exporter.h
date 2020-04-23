#ifndef TOOLS_ASC_LIN_EXPORTER_H
#define TOOLS_ASC_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "LINRecord.h"
#include "CommonOptions.h"
#include "FileInfo.h"

namespace mdf::tools::asc {

  class ASC_LIN_Exporter : public tools::shared::GenericRecordExporter<LINRecord> {
  public:
    explicit ASC_LIN_Exporter(std::ostream &output, FileInfo const &fileInfo,
      tools::shared::DisplayTimeFormat displayLocalTime);

    void writeHeader() override;

    void writeRecord(LINRecord const &record) override;

  private:
    std::string const delimiter = ";";
    std::chrono::nanoseconds firstTimeStamp;
    bool firstTimeStampSet = false;
    tools::shared::DisplayTimeFormat displayLocalTime;

    std::stringstream timeStampString;
    FileInfo const &fileInfo;

    std::string convertTimestamp(double const &timeStamp);

    std::string convertTimestamp(std::time_t const &timeStamp);
  };

}

#endif //TOOLS_ASC_LIN_EXPORTER_H
