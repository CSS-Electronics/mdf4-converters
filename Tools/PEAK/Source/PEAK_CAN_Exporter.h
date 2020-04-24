#ifndef TOOLS_PEAK_CAN_EXPORTER_H
#define TOOLS_PEAK_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <chrono>
#include <ctime>
#include <sstream>

#include "CANRecord.h"
#include "FileInfo.h"
#include "PEAK_TraceFormat.h"

namespace mdf::tools::peak {

  // Define new floating precision time base for time in ms.
  using milliseconds = std::chrono::duration<double, std::milli>;

  class PEAK_CAN_Exporter : public tools::shared::GenericRecordExporter<CANRecord> {
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

}

#endif //TOOLS_PCAP_CAN_EXPORTER_H
