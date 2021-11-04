#include "ASC_CAN_Exporter.h"

#include <iomanip>
#include <variant>

#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::asc {

    ASC_CAN_Exporter::ASC_CAN_Exporter(
        std::ostream &output,
        mdf::tools::shared::ParsedFileInfo const &fileInfo,
        tools::shared::DisplayTimeFormat displayLocalTime
        ) : GenericRecordExporter(output),
        fileInfo(fileInfo),
        displayLocalTime(displayLocalTime) {

    }

    void ASC_CAN_Exporter::writeHeader() {
        output << "date ";
        startTimePosition = output.tellp();
        output << "                           \n";
        output << "base hex  timestamps absolute\n";
        output.flush();
    }

    void ASC_CAN_Exporter::correctHeader() {
        output.seekp(startTimePosition);
        output << convertTimestamp(firstTimeStamp);
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
        if(!firstTimeStampSet) {
            firstTimeStamp = record.TimeStamp;
            firstTimeStampSet = true;
        }

        std::chrono::duration<double> deltaTime = record.TimeStamp - firstTimeStamp;
        uint32_t merged = 0;

        if(record.EDL != 0) {
            merged |= 0x00001000;
        }
        if(record.BRS != 0) {
            merged |= 0x00002000;
        }
        if(record.ESI != 0) {
            merged |= 0x00004000;
        }

        fmt::print(
            output,
            FMT_STRING("{: >11.6f} CANFD {:3d} {:s}   {:8X}{:c}                                  {:d} {:d} {:X} {:2d} {:02X}        0    0 {:8X}        0        0        0        0        0\n"),
            deltaTime.count(),
            record.BusChannel,
            (record.Dir == 0) ? "Rx" : "Tx",
            record.ID,
            (record.IDE != 0) ? 'x' : ' ',
            (record.BRS != 0) ? 1 : 0,
            (record.ESI != 0) ? 1 : 0,
            record.DLC,
            record.DataLength,
            fmt::join(record.DataBytes, " "),
            merged
        );
    }

    void ASC_CAN_Exporter::write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record) {
        if(!firstTimeStampSet) {
            firstTimeStamp = record.TimeStamp;
            firstTimeStampSet = true;
        }

        std::chrono::duration<double> deltaTime = record.TimeStamp - firstTimeStamp;

        fmt::print(
            output,
            FMT_STRING("{: >11.6f} CANFD {:3d} {:s}   {:8X}{:c}                                  0 0 {:X} {:2d}         0    0       10        0        0        0        0        0\n"),
            deltaTime.count(),
            record.BusChannel,
            (record.Dir == 0) ? "Rx" : "Tx",
            record.ID,
            record.IDE ? 'x' : ' ',
            record.DLC,
            record.DataLength
        );
    }

}
