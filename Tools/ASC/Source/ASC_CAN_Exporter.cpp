#include "ASC_CAN_Exporter.h"

#include <iomanip>
#include <variant>

#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::asc {

    ASC_CAN_Exporter::ASC_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                       tools::shared::DisplayTimeFormat displayLocalTime) : GenericRecordExporter(
        output), fileInfo(fileInfo),
                                                                                            displayLocalTime(
                                                                                                displayLocalTime) {

    }

    void ASC_CAN_Exporter::writeHeader() {
        firstTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(fileInfo.Time);
        firstTimeStampSet = true;

        output << "date " << convertTimestamp(firstTimeStamp) << "\n";
        output << "base hex  timestamps absolute\n";
        output.flush();
    }

    void ASC_CAN_Exporter::writeRecord(ASC_Record const &record) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, mdf::CAN_DataFrame>) {
                write_CAN_DataFrame(arg);
            } else if constexpr (std::is_same_v<T, mdf::CAN_RemoteFrame>) {
                write_CAN_RemoteFrame(arg);
            } else
            {
                static_assert(always_false_v<T>, "Missing visitor");
            }
        }, record);
    }

    std::string ASC_CAN_Exporter::convertTimestamp(std::time_t const &timeStamp) {
        // Reset buffer.
        timeStampString.str("");

        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

        // Convert epoch to datetime.
        std::tm const *tm = std::gmtime(&correctedTime);

        timeStampString << std::put_time(tm, "%a %b %d %I:%M:%S ");

        if (tm->tm_hour < 12) {
            timeStampString << "am";
        } else {
            timeStampString << "pm";
        }

        timeStampString << std::put_time(tm, " %Y");
        return timeStampString.str();
    }

    std::string ASC_CAN_Exporter::convertTimestamp(std::chrono::seconds const &timeStamp) {
        // Extract time and convert to right zone.
        std::time_t extractTime = timeStamp.count();
        return convertTimestamp(extractTime);
    }

    std::string ASC_CAN_Exporter::convertTimestamp(std::chrono::nanoseconds const &timeStamp) {
        // Extract time and convert to right zone.
        auto baseTime = std::chrono::duration_cast<std::chrono::seconds>(timeStamp);
        std::time_t extractTime = baseTime.count();
        return convertTimestamp(extractTime);
    }

    void ASC_CAN_Exporter::write_CAN_DataFrame(mdf::CAN_DataFrame const &record) {
        std::chrono::duration<double> deltaTime = record.TimeStamp - firstTimeStamp;

        fmt::print(
            output,
            FMT_STRING("{: >12.6f} {:d} {:x}{:c} {:s} d {:d} {:02X}\n"),
            deltaTime.count(),
            record.BusChannel,
            record.ID,
            record.IDE ? 'x' : ' ',
            (record.Dir == 0) ? "Rx" : "Tx",
            record.DLC,
            fmt::join(record.DataBytes, " ")
            );
    }

    void ASC_CAN_Exporter::write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record) {
        std::chrono::duration<double> deltaTime = record.TimeStamp - firstTimeStamp;

        fmt::print(
            output,
            FMT_STRING("{: >12.6f} {:d} {:x}{:c} {:s} r {:d}\n"),
            deltaTime.count(),
            record.BusChannel,
            record.ID,
            record.IDE ? 'x' : ' ',
            (record.Dir == 0) ? "Rx" : "Tx",
            record.DLC
        );
    }

}
