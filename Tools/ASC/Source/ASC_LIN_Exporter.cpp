#include "ASC_LIN_Exporter.h"

#include <iomanip>
#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::asc {

  ASC_LIN_Exporter::ASC_LIN_Exporter(std::ostream &output, FileInfo const &fileInfo,
    tools::shared::DisplayTimeFormat displayLocalTime) : GenericRecordExporter(output), fileInfo(fileInfo),
                                                         displayLocalTime(displayLocalTime) {

  }

  void ASC_LIN_Exporter::writeHeader() {
    firstTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(fileInfo.Time);
    firstTimeStampSet = true;

    output << "date " << convertTimestamp(fileInfo.Time.count() / 1000000000) << "\n";
    output << "base hex  timestamps absolute\n";
    output.flush();
  }

  void ASC_LIN_Exporter::writeRecord(LINRecord const &record) {
    std::chrono::duration<double> deltaTime = record.TimeStamp - firstTimeStamp;

    fmt::print(output,
      FMT_STRING("{: >12.6f} {:d} {:x}  {:s} d {:d} {:02X}\n"),
      deltaTime.count(),
      record.BusChannel,
      record.ID,
      (record.Dir == 0) ? "Rx" : "Tx",
      record.DataLength,
      fmt::join(record.DataBytes, " ")
    );
  }

  std::string ASC_LIN_Exporter::convertTimestamp(std::time_t const &timeStamp) {
    // Reset buffer.
    timeStampString.str("");

    // Convert to right zone.
    std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

    // Convert epoch to datetime.
    std::tm const *tm = std::gmtime(&correctedTime);

    timeStampString << std::put_time(tm, "%a %b %d %I:%M:%S %p %Y");
    return timeStampString.str();
  }

  std::string ASC_LIN_Exporter::convertTimestamp(double const &timeStamp) {
    // Convert epoch to datetime.
    auto t = static_cast<std::time_t>(timeStamp);
    return convertTimestamp(t);
  }

}
