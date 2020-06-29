#include "PEAK_TraceFormat.h"

#include <sstream>
#include <iomanip>
#include <fmt/ostream.h>

namespace mdf::tools::peak {

    std::istream &operator>>(std::istream &input, PEAK_TraceFormat &traceFormat) {
        // Read current token.
        std::string token;
        input >> token;

        if ((token == "11") || (token == "1.1")) {
            traceFormat = PEAK_TraceFormat::VERSION_1_1;
        } else if ((token == "21") || (token == "2.1")) {
            traceFormat = PEAK_TraceFormat::VERSION_2_1;
        } else {
            std::stringstream ss;
            fmt::print(
                ss,
                FMT_STRING(
                    "Value for trace format must be \"{:s}\", \"{:s}\", \"{:s}\" or \"{:s}\". You supplied \"{:s}\""),
                "11",
                "1.1",
                "21",
                "2.1",
                token
            );

            throw std::invalid_argument(ss.str());
        }

        return input;
    }

    std::ostream &operator<<(std::ostream &output, PEAK_TraceFormat const &traceFormat) {
        switch (traceFormat) {
            case PEAK_TraceFormat::VERSION_1_1:
                output << "1.1";
                break;
            case PEAK_TraceFormat::VERSION_2_1:
                output << "2.1";
                break;
            default:
                break;
        }
        return output;
    }

}
