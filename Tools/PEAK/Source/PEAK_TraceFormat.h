#ifndef TOOLS_PEAK_TRACEFORMAT_H
#define TOOLS_PEAK_TRACEFORMAT_H

#include <iostream>

namespace mdf::tools::peak {

  enum struct PEAK_TraceFormat {
    VERSION_1_1,
    VERSION_2_1
  };

  std::istream& operator>>(std::istream& input, PEAK_TraceFormat& traceFormat);
  std::ostream& operator<<(std::ostream& output, PEAK_TraceFormat const& traceFormat);

}

#endif //TOOLS_PEAK_TRACEFORMAT_H
