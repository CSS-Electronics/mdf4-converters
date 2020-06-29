#include "CAN_RemoteFrame.h"

#include <fmt/ostream.h>

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, CAN_RemoteFrame const &record) {
        if (record.IDE) {
            fmt::print(
                stream,
                FMT_STRING("{:.6f} - {:8X} - {:d}"),
                std::chrono::duration<double>(record.TimeStamp).count(),
                record.ID,
                record.DLC
            );
        } else {
            fmt::print(
                stream,
                FMT_STRING("{:.6f} - {:4X} - {:d}"),
                std::chrono::duration<double>(record.TimeStamp).count(),
                record.ID,
                record.DLC
            );
        }

        return stream;
    }

}
