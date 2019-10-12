#include "GenericDataRecord.h"

namespace mdf {

    std::ostream& operator<<(std::ostream& stream, const GenericDataRecord& record) {
        stream << record.recordID << ": " << record.data.size();

        return stream;
    }

}
