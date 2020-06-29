#include "PEAK_CAN_Exporter_1_1.h"

namespace mdf::tools::peak {

    PEAK_CAN_Exporter::PEAK_CAN_Exporter(std::ostream &output, FileInfo const &fileInfo) : GenericRecordExporter(
        output), fileInfo(fileInfo) {
        recordCounter = 0;
    }

    milliseconds PEAK_CAN_Exporter::convertTimestampToRelative(std::chrono::nanoseconds timeStamp) const {
        // Take the difference between the first timestamp and this.
        std::chrono::nanoseconds difference = timeStamp - headerTimeStamp;

        milliseconds result(difference);

        return result;
    }

}
