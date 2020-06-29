#include "LIN_ReceiveError.h"

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, LIN_ReceiveError const &record) {
        stream << "A LIN record\n";

        return stream;
    }

}
