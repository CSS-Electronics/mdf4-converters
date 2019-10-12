#include "Version.h"

namespace interfaces {

    std::ostream& operator<< (std::ostream& stream, const Version& version) {
        stream << version.major << "." << version.minor << "." << version.patch;

        return stream;
    }

}
