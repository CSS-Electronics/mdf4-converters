#include <cmath>
#include <iomanip>
#include <fmt/printf.h>
#include <fmt/ostream.h>

#include "PEAK_CAN_Exporter_2_1.h"

namespace mdf::tools::peak {

  PEAK_CAN_Exporter_2_1::PEAK_CAN_Exporter_2_1(std::ostream &output, FileInfo const &fileInfo) : PEAK_CAN_Exporter(
    output, fileInfo) {
  }

  static double const daysBetweenCenturyAndEpoch = 25568.0f;

  void PEAK_CAN_Exporter_2_1::correctHeader() {
    if (timeStampSet) {
      output.flush();
      output.seekp(startTimePosition);

      // Convert the time to days since 30th December, 1899. Utilize that there are 25568.0 days between this
      // and epoch start time.
      double nsInADay = 24 * 60 * 60 * 1E9;

      // Days since 1970.
      double daysSinceEpoch = headerTimeStamp.count() / nsInADay;
      double headerDate = daysBetweenCenturyAndEpoch + daysSinceEpoch;

      double days = 0;
      double fractional = std::modf(headerDate, &days);

      output << std::setw(5) << static_cast<long long>(days) << ".";
      output << std::setw(10) << std::setfill('0') << static_cast<long long>(fractional * 1E10) << " ";
      output << std::setfill(' ');
    }
  }

  void PEAK_CAN_Exporter_2_1::writeHeader() {
    // Write static information.
    fmt::print(output, FMT_STRING(";$FILEVERSION=2.1\n"));

    // Write a placeholder for the start time, since this is not known until the first frame is processed.
    fmt::print(output, FMT_STRING(";$STARTTIME="));
    output.flush();
    startTimePosition = output.tellp();
    fmt::print(output, FMT_STRING("0000000000000000\n"));

    // Print columns present in the format.
    fmt::print(output, FMT_STRING(";$COLUMNS=N,O,T,B,I,d,L,D\n"));
  }

  void PEAK_CAN_Exporter_2_1::writeRecord(CANRecord const &record) {
    // If this is the first record, extract the timestamp from this.
    if (!timeStampSet) {
      timeStampSet = true;
      headerTimeStamp = record.TimeStamp;
    }

    // Columns:
    // Record number (In this file).
    // Time offset from start of file in ms.us.
    // Message type.
    // Bus number.
    // CAN ID.
    // Direction of data.
    // DLC field.
    // Data.
    double timeStamp = convertTimestampToRelative(record.TimeStamp);

    std::string messageType;
    if (record.EDL) {
      if (record.BRS) {
        messageType = "FB";
      } else {
        messageType = "FD";
      }
    } else {
      messageType = "DT";
    }

    if(record.IDE) {
      fmt::print(
        output,
        FMT_STRING("{:7d} {:13.3f} {:s} {:d} {:08X} {:s} {:02d} {:02X}\n"),
        recordCounter++,
        timeStamp,
        messageType,
        record.BusChannel,
        record.ID,
        (record.Dir == 0) ? "Rx" : "Tx",
        record.DLC,
        fmt::join(record.DataBytes, " ")
      );
    } else {
      fmt::print(
        output,
        FMT_STRING("{:7d} {:13.3f} {:s} {:d}     {:04X} {:s} {:02d} {:02X}\n"),
        recordCounter++,
        timeStamp,
        messageType,
        record.BusChannel,
        record.ID,
        (record.Dir == 0) ? "Rx" : "Tx",
        record.DLC,
        fmt::join(record.DataBytes, " ")
      );
    }
  }

}
