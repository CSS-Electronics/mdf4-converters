#ifndef TOOLS_CSV_CAN_COMMON_H
#define TOOLS_CSV_CAN_COMMON_H

#include <variant>

#include "CAN_DataFrame.h"
#include "CAN_RemoteFrame.h"

namespace mdf::tools::csv {

    typedef std::variant<const mdf::CAN_DataFrame, const mdf::CAN_RemoteFrame> CSV_Record;
    
    template<typename T> inline constexpr bool always_false_v = false;

}

#endif //TOOLS_CSV_CAN_COMMON_H
