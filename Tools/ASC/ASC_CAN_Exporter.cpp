#include "ASC_CAN_Exporter.h"

#include <iomanip>

#include "TimeConverter.h"

namespace tools::asc {

    ASC_CAN_Exporter::ASC_CAN_Exporter(std::ostream& output, FileInfo_t const& fileInfo, tools::shared::DisplayTimeFormat displayLocalTime) : GenericRecordExporter(output), fileInfo(fileInfo), displayLocalTime(displayLocalTime) {

    }

    void ASC_CAN_Exporter::writeHeader() {
        output << "date " << convertTimestamp(fileInfo.Time) << "\n";
        output << "base hex  timestamps absolute\n";

        firstTimeStamp = fileInfo.Time;
        firstTimeStampSet = true;

        output.flush();
    }

    void ASC_CAN_Exporter::writeRecord(CAN_DataFrame_t record) {
        char extended = ' ';

        if(record.IDE) {
            extended = 'x';
        }

        output << std::setw(5 + 1 + 6) << std::fixed << std::setprecision(6) << (record.TimeStamp * 1E-9) - firstTimeStamp;
        output << " " << static_cast<unsigned int>(record.BusChannel) << " " << std::hex << record.ID << std::dec << extended;

        if(record.Dir == 0) {
            output << " Rx ";
        } else {
            output << " Tx ";
        }
        output << "d " << static_cast<int>(record.DLC) << " ";

        // Set format for writing data.
        output << std::hex << std::setfill('0') << std::uppercase;

        // Loop over data.
        for(int i = 0; i < record.DataLength; ++i) {
            output << std::setw(2) << static_cast<int>(record.DataBytes[i]) << " ";
        }

        // Reset format.
        output << std::dec << std::setfill(' ') << std:: nouppercase << std::endl;
    }

    std::string ASC_CAN_Exporter::convertTimestamp(std::time_t const& timeStamp) {
        // Reset buffer.
        timeStampString.str("");

        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

        // Convert epoch to datetime.
        std::tm const* tm = std::gmtime(&correctedTime);

        timeStampString << std::put_time(tm, "%a %b %d %I:%M:%S ");

        if(tm->tm_hour < 12) {
            timeStampString << "am";
        } else {
            timeStampString << "pm";
        }

        timeStampString << std::put_time(tm, " %Y");
        return timeStampString.str();
    }

    std::string ASC_CAN_Exporter::convertTimestamp(double const& timeStamp) {
        // Convert epoch to datetime.
        auto t = static_cast<std::time_t>(timeStamp);
        return convertTimestamp(t);
    }

}
