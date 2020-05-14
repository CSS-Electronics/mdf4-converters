#ifndef MDFSIMPLECONVERTERS_STATUSCODE_H
#define MDFSIMPLECONVERTERS_STATUSCODE_H

#include <cstdint>

namespace mdf::tools::shared {

  /*!Possible statuscodes. Since the value returned on POSIX from a process is limited to an unsigned 8 bit variable,
   * the scope is limited. If the MSB is set, it denotes a single error code by parsing the remaining 7 bits as an
   * unsigned integer. Else, the first 7 bits are used as flags.
   *
   */
  enum struct StatusCode : int32_t {
    // Status flags, not touching the MSB.
    NoErrors = 0,
    MissingFile = 0x1u << 0u,
    DecryptionError = 0x1u << 1u,
    DecompressionError = 0x1u << 2u,
    DecodingError = 0x1u << 3u,

    // Error codes, setting the MSB.
    CriticalError = (0x80u + 0x01u),
    MissingArgument = (0x80u + 0x02),
    InvalidInputArgument = (0x80u + 0x03u),
    InputArgumentParsingError = (0x80u + 0x04u),
    ConfigurationFileParseError = (0x80u + 0x05u),
    UnrecognizedOption = (0x80u + 0x06u),
  };

  StatusCode operator&(StatusCode const &lhs, StatusCode const &rhs);

  StatusCode operator|(StatusCode const &lhs, StatusCode const &rhs);

  StatusCode &operator|=(StatusCode &lhs, StatusCode const &rhs);

}

#endif //MDFSIMPLECONVERTERS_STATUSCODE_H
