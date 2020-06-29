#include "LIN_Frame.h"

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, LIN_Frame const &record) {
        stream << "A LIN record\n";

        return stream;
    }

}
