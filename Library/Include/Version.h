#ifndef MDFSIMPLECONVERTERS_VERSION_H
#define MDFSIMPLECONVERTERS_VERSION_H

#include <iostream>
#include <optional>

namespace mdf {

  struct Version {
    unsigned int major;
    unsigned int minor;
    std::optional<unsigned int> patch;
  };

  std::ostream &operator<<(std::ostream &stream, const Version &version);

}

#endif //MDFSIMPLECONVERTERS_VERSION_H
