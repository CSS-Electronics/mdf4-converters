#include "CAN_DataFrame.h"

#include <fmt/ostream.h>

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, CAN_DataFrame const &record) {
        if (record.IDE) {
            fmt::print(
                stream,
                FMT_STRING("{:.6f} - {:8X} - {:d} - {:02X}"),
                std::chrono::duration<double>(record.TimeStamp).count(),
                record.ID,
                record.DLC,
                fmt::join(record.DataBytes, "")
            );
        } else {
            fmt::print(
                stream,
                FMT_STRING("{:.6f} - {:4X} - {:d} - {:02X}"),
                std::chrono::duration<double>(record.TimeStamp).count(),
                record.ID,
                record.DLC,
                fmt::join(record.DataBytes, "")
            );
        }

        return stream;
    }

}
