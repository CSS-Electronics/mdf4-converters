#include "CSV_CAN_Exporter.h"

#include <iomanip>

namespace tools::csv {

    CSV_CAN_Exporter::CSV_CAN_Exporter(std::ostream &output, std::string delimiter) : GenericRecordExporter(output), delimiter(std::move(delimiter)) {

    }

    void CSV_CAN_Exporter::writeHeader() {
        output << "TimestampEpoch" << delimiter;
        output << "BusChannel" << delimiter;
        output << "ID" << delimiter;
        output << "IDE" << delimiter;
        output << "DLC" << delimiter;
        output << "DataLength" << delimiter;
        output << "Dir" << delimiter;
        output << "EDL" << delimiter;
        output << "BRS" << delimiter;
        output << "DataBytes" << std::endl;
    }

    void CSV_CAN_Exporter::writeRecord(CAN_DataFrame_t record) {
        output << std::fixed << std::setprecision(6) << record.TimeStamp * 1E-9 << delimiter;
        output << static_cast<int>(record.BusChannel) << delimiter;
        output << std::hex << std::uppercase << record.ID << std::dec << std::nouppercase << delimiter;
        output << record.IDE << delimiter;
        output << static_cast<int>(record.DLC) << delimiter;
        output << static_cast<int>(record.DataLength) << delimiter;
        output << record.Dir << delimiter;
        output << record.EDL << delimiter;
        output << record.BRS << delimiter;

        // Set format for writing data.
        output << std::hex << std::setfill('0') << std::uppercase;

        // Loop over data.
        for(int i = 0; i < record.DataLength; ++i) {
            output << std::setw(2) << static_cast<int>(record.DataBytes[i]);
        }

        // Reset format.
        output << std::dec << std::setfill(' ') << std:: nouppercase << std::endl;
    }

}
