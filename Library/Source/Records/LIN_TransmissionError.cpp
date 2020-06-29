#include "LIN_TransmissionError.h"

namespace mdf {

    std::ostream &operator<<(std::ostream &stream, LIN_TransmissionError const &record) {
        stream << "A LIN record\n";

        return stream;
    }

}
