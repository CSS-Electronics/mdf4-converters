#ifndef MDFSORTER_VERSION22_H
#define MDFSORTER_VERSION22_H

#include <iostream>

namespace interfaces {

    struct Version {
        const int major;
        const int minor;
        const int patch;
    };

    std::ostream& operator<< (std::ostream& stream, const Version& version);

}

#endif //MDFSORTER_VERSION_H
