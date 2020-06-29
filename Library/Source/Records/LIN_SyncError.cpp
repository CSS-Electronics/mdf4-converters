#include "LIN_SyncError.h"

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, LIN_SyncError const &record) {
        stream << "A LIN record\n";

        return stream;
    }

}
