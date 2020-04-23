#include "PEAK_CAN_Exporter_1_1.h"

namespace mdf::tools::peak {

  PEAK_CAN_Exporter::PEAK_CAN_Exporter(std::ostream &output, FileInfo const &fileInfo) : GenericRecordExporter(
    output), fileInfo(fileInfo) {
    recordCounter = 0;
  }

  double PEAK_CAN_Exporter::convertTimestampToRelative(std::chrono::nanoseconds timeStamp) const {
    // Take the difference between the first timestamp and this.
    std::chrono::nanoseconds difference = timeStamp - headerTimeStamp;

    double differenceInUs = difference.count() * 1E-3;

    return differenceInUs;
  }

}
