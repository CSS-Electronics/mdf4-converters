#include "SocketCAN_LIN_Exporter.h"

#include <fmt/chrono.h>
#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::socketcan {

    SocketCAN_LIN_Exporter::SocketCAN_LIN_Exporter(std::ostream &output,
                                                   mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                                   tools::shared::DisplayTimeFormat displayLocalTime)
        : GenericRecordExporter(output), fileInfo(fileInfo), displayLocalTime(displayLocalTime) {

    }

    void SocketCAN_LIN_Exporter::writeHeader() {
        // Do nothing.
    }

    void SocketCAN_LIN_Exporter::writeRecord(LIN_Frame const &record) {
        // The SocketCAN format is as follows:
        // Standard: (Timestamp) interface ID#XX
        // FD: (Timestamp) interface ID##FXX
        // * Timestamp is epoch with microseconds as a float.
        // * Interface corresponds to "can0" and "can1".
        // * ID is either 3 hex characters or 8 hex characters, depending on the status of the extended ID. Fill with 0.
        // * F is flags in the FD case, not used in the classic case.
        // * XX corresponds to data in hex. The length of this must match the DLC, such that a DLC of 2 corresponds to
        //   4 characters etc.

        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, record.TimeStamp.count(),
                                                                    fileInfo);

        fmt::print(
            output,
            FMT_STRING("({:0.6f}) lin{:d} {:03X}#{:02X}\n"),
            correctedTime * 1E-9,
            record.BusChannel - 1,
            record.ID,
            fmt::join(record.DataBytes, "")
        );
    }

}
