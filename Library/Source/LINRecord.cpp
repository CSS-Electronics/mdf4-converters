#include "LINRecord.h"

namespace mdf {

  std::ostream &operator<<(std::ostream & stream, LINRecord const& record) {
    stream << "A LIN record\n";

    return stream;
  }

}
