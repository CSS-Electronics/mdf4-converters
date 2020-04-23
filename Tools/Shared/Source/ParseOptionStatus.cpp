#include "ParseOptionStatus.h"

#include <type_traits>

namespace mdf::tools::shared {

  ParseOptionStatus operator&(ParseOptionStatus const &lhs, ParseOptionStatus const &rhs) {
    // Determine the underlying type.
    using T = std::underlying_type_t<ParseOptionStatus>;

    // Perform casts to integer types, perform the operation, and cast back.
    return static_cast<ParseOptionStatus>(static_cast<T>(lhs) & static_cast<T>(rhs));
  }

  ParseOptionStatus operator|(ParseOptionStatus const &lhs, ParseOptionStatus const &rhs) {
    // Determine the underlying type.
    using T = std::underlying_type_t<ParseOptionStatus>;

    // Perform casts to integer types, perform the operation, and cast back.
    return static_cast<ParseOptionStatus>(static_cast<T>(lhs) | static_cast<T>(rhs));
  }

  ParseOptionStatus &operator|=(ParseOptionStatus &lhs, ParseOptionStatus const &rhs) {
    lhs = lhs | rhs;
    return lhs;
  }

  ParseOptionStatus &operator&=(ParseOptionStatus &lhs, ParseOptionStatus const &rhs) {
    lhs = lhs & rhs;
    return lhs;
  }

}
