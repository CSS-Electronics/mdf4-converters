#include "CSV_CAN_Exporter.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace mdf::tools::csv {

    CSV_CAN_Exporter::CSV_CAN_Exporter(std::ostream &output, std::string delimiter) :
        GenericRecordExporter(output),
        delimiter(std::move(delimiter)) {
        //
    }

    void CSV_CAN_Exporter::writeHeader() {
        fmt::print(output, "TimestampEpoch{}", delimiter);
        fmt::print(output, "BusChannel{}", delimiter);
        fmt::print(output, "ID{}", delimiter);
        fmt::print(output, "IDE{}", delimiter);
        fmt::print(output, "DLC{}", delimiter);
        fmt::print(output, "DataLength{}", delimiter);
        fmt::print(output, "Dir{}", delimiter);
        fmt::print(output, "EDL{}", delimiter);
        fmt::print(output, "BRS{}", delimiter);
        fmt::print(output, "DataBytes\n");
    }

    void CSV_CAN_Exporter::writeRecord(CSV_Record const &record) {
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

    void CSV_CAN_Exporter::write_CAN_DataFrame(mdf::CAN_DataFrame const &record) {
        // TODO: Switch timestamp to locale aware solution once libfmt implements ":Ln".
        //       See https://github.com/fmtlib/fmt/issues/1381

        fmt::print(
            output,
            FMT_STRING("{:.6f}{:s}"
                       "{:d}{:s}"
                       "{:X}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:02X}\n"),
            std::chrono::duration<double>(record.TimeStamp).count(), delimiter,
            record.BusChannel, delimiter,
            record.ID, delimiter,
            record.IDE, delimiter,
            record.DLC, delimiter,
            record.DataLength, delimiter,
            record.Dir, delimiter,
            record.EDL, delimiter,
            record.BRS, delimiter,
            fmt::join(record.DataBytes, "")
        );
    }

    void CSV_CAN_Exporter::write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record) {
        // TODO: Switch timestamp to locale aware solution once libfmt implements ":Ln".
        //       See https://github.com/fmtlib/fmt/issues/1381

        fmt::print(
            output,
            FMT_STRING("{:.6f}{:s}"
                       "{:d}{:s}"
                       "{:X}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "{:d}{:s}"
                       "0{:s}"
                       "0{:s}"
                       "\n"),
            std::chrono::duration<double>(record.TimeStamp).count(), delimiter,
            record.BusChannel, delimiter,
            record.ID, delimiter,
            record.IDE, delimiter,
            record.DLC, delimiter,
            record.DataLength, delimiter,
            record.Dir, delimiter,
            delimiter,
            delimiter
        );
    }

}
