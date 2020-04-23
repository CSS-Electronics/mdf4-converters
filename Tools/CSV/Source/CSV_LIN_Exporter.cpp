#include "CSV_LIN_Exporter.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace mdf::tools::csv {

  CSV_LIN_Exporter::CSV_LIN_Exporter(std::ostream &output, std::string delimiter) :
    GenericRecordExporter(output),
    delimiter(std::move(delimiter)) {
    //
  }

  void CSV_LIN_Exporter::writeHeader() {
    fmt::print(
      output,
      FMT_STRING("TimestampEpoch{:s}"
                 "BusChannel{:s}"
                 "ID{:s}"
                 "DataLength{:s}"
                 "Dir{:s}"
                 "DataBytes\n"
      ), delimiter,
      delimiter,
      delimiter,
      delimiter,
      delimiter
      );
  }

  void CSV_LIN_Exporter::writeRecord(LINRecord const& record) {
    // TODO: Switch timestamp to locale aware solution once libfmt implements ":Ln".
    //       See https://github.com/fmtlib/fmt/issues/1381

    fmt::print(
      output,
      FMT_STRING("{:.6f}{:s}"
                 "{}{:s}"
                 "{:X}{:s}"
                 "{:d}{:s}"
                 "{:d}{:s}"
                 "{:X}\n"),
      std::chrono::duration<double>(record.TimeStamp).count(), delimiter,
      record.BusChannel, delimiter,
      record.ID, delimiter,
      record.DataLength, delimiter,
      record.Dir, delimiter,
      fmt::join(record.DataBytes.cbegin(), record.DataBytes.cbegin() + record.DataLength, "")
    );
  }

}
