#include <cmath>
#include <iomanip>

#include <fmt/printf.h>
#include <fmt/ostream.h>

#include "PEAK_CAN_Exporter_1_1.h"

namespace mdf::tools::peak {

    PEAK_CAN_Exporter_1_1::PEAK_CAN_Exporter_1_1(std::ostream &output, FileInfo const &fileInfo) : PEAK_CAN_Exporter(
        output, fileInfo) {
    }

    static double const daysBetweenCenturyAndEpoch = 25568.0f;

    void PEAK_CAN_Exporter_1_1::correctHeader() {
        if (timeStampSet) {
            output.flush();
            output.seekp(startTimePosition);

            // Convert the time to days since 30th December, 1899. Utilize that there are 25568.0 days between this
            // and epoch start time.
            double nsInADay = 24 * 60 * 60 * 1E9;

            // Days since 1970.
            double daysSinceEpoch = headerTimeStamp.count() / nsInADay;
            double headerDate = daysBetweenCenturyAndEpoch + daysSinceEpoch;

            double days = 0;
            double fractional = std::modf(headerDate, &days);

            output << std::setw(5) << static_cast<long long>(days) << ".";
            output << std::setw(10) << std::setfill('0') << static_cast<long long>(fractional * 1E10) << " ";
            output << std::setfill(' ');
        }
    }

    void PEAK_CAN_Exporter_1_1::writeHeader() {
        // Write static information.
        fmt::print(output, FMT_STRING(";$FILEVERSION=1.1\n"));

        // Write a placeholder for the start time, since this is not known until the first frame is processed.
        fmt::print(output, FMT_STRING(";$STARTTIME="));
        output.flush();
        startTimePosition = output.tellp();
        fmt::print(output, FMT_STRING("0000000000000000\n"));
    }

    void PEAK_CAN_Exporter_1_1::writeRecord(PEAK_Record const &record) {
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

    void PEAK_CAN_Exporter_1_1::write_CAN_DataFrame(mdf::CAN_DataFrame const &record) {
        // If this is an FD frame, ignore it.
        if (record.EDL) {
            return;
        }

        // If this is the first record, extract the timestamp from this.
        if (!timeStampSet) {
            timeStampSet = true;
            headerTimeStamp = record.TimeStamp;
        }

        // Columns:
        // Record number (In this file).
        // Time offset from start of file in ms.us.
        // Message type.
        // CAN ID.
        // DLC field.
        // Data.
        milliseconds timeStamp = convertTimestampToRelative(record.TimeStamp);

        // Select print statement based on ID type.
        if (record.IDE) {
            fmt::print(
                output,
                FMT_STRING("{:6d}) {:11.1f} {:s}    {:08X} {:d} {:02X}\n"),
                recordCounter++,
                timeStamp.count(),
                (record.Dir == 0) ? "Rx" : "Tx",
                record.ID,
                record.DLC,
                fmt::join(record.DataBytes, " ")
                );
        } else {
            fmt::print(
                output,
                FMT_STRING("{:6d}) {:11.1f} {:s}        {:04X} {:d} {:02X}\n"),
                recordCounter++,
                timeStamp.count(),
                (record.Dir == 0) ? "Rx" : "Tx",
                record.ID,
                record.DLC,
                fmt::join(record.DataBytes, " ")
                );
        }
    }

    void PEAK_CAN_Exporter_1_1::write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record) {
        // If this is the first record, extract the timestamp from this.
        if (!timeStampSet) {
            timeStampSet = true;
            headerTimeStamp = record.TimeStamp;
        }

        // Columns:
        // Record number (In this file).
        // Time offset from start of file in ms.us.
        // Message type.
        // CAN ID.
        // DLC field.
        // RTR.
        milliseconds timeStamp = convertTimestampToRelative(record.TimeStamp);

        // Select print statement based on ID type.
        if (record.IDE) {
            fmt::print(
                output,
                FMT_STRING("{:6d}) {:11.1f} {:s}    {:08X} {:d} RTR\n"),
                recordCounter++,
                timeStamp.count(),
                (record.Dir == 0) ? "Rx" : "Tx",
                record.ID,
                record.DLC
                );
        } else {
            fmt::print(
                output,
                FMT_STRING("{:6d}) {:11.1f} {:s}        {:04X} {:d} RTR\n"),
                recordCounter++,
                timeStamp.count(),
                (record.Dir == 0) ? "Rx" : "Tx",
                record.ID,
                record.DLC
                );
        }
    }

}
