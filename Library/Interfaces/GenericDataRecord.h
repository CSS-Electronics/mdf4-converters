#ifndef MDFSORTER_GENERICDATARECORD_H
#define MDFSORTER_GENERICDATARECORD_H

#include <cstdint>
#include <ostream>
#include <vector>

namespace mdf {

    struct GenericDataRecord {
        uint64_t recordID;
        std::vector<uint8_t> data;
    };

    std::ostream& operator<<(std::ostream& stream, const GenericDataRecord& record);
}

#endif //MDFSORTER_GENERICDATARECORD_H
