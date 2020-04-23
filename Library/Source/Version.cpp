#include "Version.h"

#include <fmt/ostream.h>

namespace mdf {

  std::ostream& operator<< (std::ostream& stream, const Version& version) {
    // Determine if the patch is included.
    if(version.patch.has_value()) {
      fmt::print(
        stream,
        FMT_STRING("{:02d}.{:02d}.{:02d}"),
        version.major,
        version.minor,
        version.patch.value()
      );
    } else {
      fmt::print(
        stream,
        FMT_STRING("{:02d}.{:02d}"),
        version.major,
        version.minor
      );
    }

    return stream;
  }

}
