#include "CSV_LIN_Exporter.h"

#include <iomanip>

namespace tools::csv {

    CSV_LIN_Exporter::CSV_LIN_Exporter(std::ostream &output, std::string delimiter) : GenericRecordExporter(output), delimiter(std::move(delimiter)) {

    }

    void CSV_LIN_Exporter::writeHeader() {
        output << "TimestampEpoch" << delimiter;
        output << "BusChannel" << delimiter;
        output << "ID" << delimiter;
        output << "DataLength" << delimiter;
        output << "Dir" << delimiter;
        output << "DataBytes" << std::endl;
    }

    void CSV_LIN_Exporter::writeRecord(LIN_Frame_t record) {
        output << std::fixed << std::setprecision(6) << record.TimeStamp * 1E-9 << delimiter;
        output << static_cast<int>(record.BusChannel) << delimiter;
        output << std::hex << std::uppercase << record.ID << std::dec << std::nouppercase << delimiter;
        output << static_cast<int>(record.DataLength) << delimiter;
        output << record.Dir << delimiter;

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
