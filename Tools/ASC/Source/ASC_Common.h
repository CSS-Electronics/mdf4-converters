#ifndef TOOLS_ASC_CAN_COMMON_H
#define TOOLS_ASC_CAN_COMMON_H

#include <variant>

#include "CAN_DataFrame.h"
#include "CAN_RemoteFrame.h"

namespace mdf::tools::asc {

    typedef std::variant<const mdf::CAN_DataFrame, const mdf::CAN_RemoteFrame> ASC_Record;
    
    template<typename T> inline constexpr bool always_false_v = false;

}

#endif //TOOLS_ASC_CAN_COMMON_H
