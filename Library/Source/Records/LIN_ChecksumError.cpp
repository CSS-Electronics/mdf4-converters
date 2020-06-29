#include "LIN_ChecksumError.h"

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, LIN_ChecksumError const &record) {
        stream << "A LIN record\n";

        return stream;
    }

}
