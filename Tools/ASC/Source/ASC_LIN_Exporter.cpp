#include "ASC_LIN_Exporter.h"

#include <iomanip>
#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::asc {

    ASC_LIN_Exporter::ASC_LIN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                       tools::shared::DisplayTimeFormat displayLocalTime) : GenericRecordExporter(
        output), fileInfo(fileInfo),
                                                                                            displayLocalTime(
                                                                                                displayLocalTime) {

    }

    void ASC_LIN_Exporter::writeHeader() {
        output << "date ";
        startTimePosition = output.tellp();
        output << "                           \n";
        output << "base hex  timestamps absolute\n";
        output << "Begin Triggerblock\n";
        output.flush();
    }

    void ASC_LIN_Exporter::writeFooter() {
        output << "End Triggerblock\n";
        output.flush();
    }

    void ASC_LIN_Exporter::correctHeader() {
        output.seekp(startTimePosition);
        output << convertTimestamp(firstTimeStamp);
    }

    void ASC_LIN_Exporter::writeRecord(LIN_Frame const &record) {
        if(!firstTimeStampSet) {
            firstTimeStamp = record.TimeStamp;
            firstTimeStampSet = true;
        }

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

    std::string ASC_LIN_Exporter::convertTimestamp(std::chrono::seconds const &timeStamp) {
        // Extract time and convert to right zone.
        std::time_t extractTime = timeStamp.count();
        return convertTimestamp(extractTime);
    }

    std::string ASC_LIN_Exporter::convertTimestamp(std::chrono::nanoseconds const &timeStamp) {
        // Extract time and convert to right zone.
        auto baseTime = std::chrono::duration_cast<std::chrono::seconds>(timeStamp);
        std::time_t extractTime = baseTime.count();
        return convertTimestamp(extractTime);
    }

}
