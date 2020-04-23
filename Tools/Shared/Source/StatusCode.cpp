#include "StatusCode.h"

#include <type_traits>

namespace mdf::tools::shared {

  StatusCode operator&(StatusCode const &lhs, StatusCode const &rhs) {
    // Determine the underlying type.
    using T = std::underlying_type_t<StatusCode>;

    // Perform casts to integer types, perform the operation, and cast back.
    return static_cast<StatusCode>(static_cast<T>(lhs) & static_cast<T>(rhs));
  }

  StatusCode operator|(StatusCode const &lhs, StatusCode const &rhs) {
    // Determine the underlying type.
    using T = std::underlying_type_t<StatusCode>;

    // Perform casts to integer types, perform the operation, and cast back.
    return static_cast<StatusCode>(static_cast<T>(lhs) | static_cast<T>(rhs));
  }

  StatusCode &operator|=(StatusCode &lhs, StatusCode const &rhs) {
    lhs = lhs | rhs;
    return lhs;
  }

}
