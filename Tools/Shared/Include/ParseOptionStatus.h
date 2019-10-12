#ifndef TOOLS_SHARED_PARSEOPTIONSTATUS_H
#define TOOLS_SHARED_PARSEOPTIONSTATUS_H

#include <cstdint>

namespace tools::shared {

    enum class ParseOptionStatus : uint32_t {
        NoError = 0,
        NoInputFiles = 0x1u << 1u,
        DisplayHelp = 0x1u << 2u,
        DisplayVersion = 0x1u << 3u,
        UnrecognizedOption = 0x1u << 4u,
    };

    ParseOptionStatus operator&(ParseOptionStatus const& lhs, ParseOptionStatus const&rhs);
    ParseOptionStatus operator|(ParseOptionStatus const& lhs, ParseOptionStatus const&rhs);
    ParseOptionStatus& operator|=(ParseOptionStatus & lhs, ParseOptionStatus const&rhs);

}

#endif //TOOLS_SHARED_PARSEOPTIONSTATUS_H
