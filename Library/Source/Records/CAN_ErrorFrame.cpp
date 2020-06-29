#include "CAN_ErrorFrame.h"

#include <fmt/ostream.h>

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, CAN_ErrorFrame const &record) {
        fmt::print(
            stream,
            FMT_STRING("{:.6f} - {:d} - {:d}"),
            std::chrono::duration<double>(record.TimeStamp).count(),
            record.BusChannel,
            record.ErrorType
        );

        return stream;
    }

}
