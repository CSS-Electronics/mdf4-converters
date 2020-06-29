#ifndef MDFSIMPLECONVERTERS_VERSION_H
#define MDFSIMPLECONVERTERS_VERSION_H

#include <iostream>
#include <optional>

namespace mdf {

    struct Version {
        unsigned int major;
        unsigned int minor;
        std::optional<unsigned int> patch;

        constexpr Version(unsigned int major, unsigned int minor, std::optional<unsigned int> patch) : major(major), minor(minor), patch(patch) {

        }
        Version() = default;
    };

    std::ostream &operator<<(std::ostream &stream, const Version &version);

}

#endif //MDFSIMPLECONVERTERS_VERSION_H
