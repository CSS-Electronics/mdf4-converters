#include "SocketCAN_LIN_Exporter.h"

#include <iomanip>

#include "TimeConverter.h"

namespace tools::socketcan {

    SocketCAN_LIN_Exporter::SocketCAN_LIN_Exporter(std::ostream &output, FileInfo_t const& fileInfo, tools::shared::DisplayTimeFormat displayLocalTime) : GenericRecordExporter(output), fileInfo(fileInfo), displayLocalTime(displayLocalTime) {

    }

    void SocketCAN_LIN_Exporter::writeHeader() {
        // Do nothing.
    }

    void SocketCAN_LIN_Exporter::writeRecord(LIN_Frame_t record) {
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
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, record.TimeStamp, fileInfo);

        output << "(" << std::fixed << std::setprecision(6) << correctedTime * 1E-9 << ") ";
        output << "lin" << static_cast<int>(record.BusChannel - 1) << " ";

        output << std::hex << std::uppercase << std::setfill('0');
        output << std::setw(3) << record.ID << std::dec << "#";

        // DLC.
        // NOTE: Seems to be optional, and can cause errors, so disabled for now.
        // output << static_cast<int>(record.DLC);

        // Data.
        output << std::hex << std::setfill('0');
        for(int i = 0; i < record.DataLength; ++i) {
            output << std::setw(2) << static_cast<int>(record.DataBytes[i]);
        }

        // Reset format.
        output << std::dec << std::nouppercase << std::endl;
    }

}
