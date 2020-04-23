#ifndef MDFSIMPLECONVERTERS_MDFHEADER_H
#define MDFSIMPLECONVERTERS_MDFHEADER_H

#include <cstdint>

#include "MdfBlockTypes.h"

namespace mdf {

    struct MdfHeader {
        MdfBlockType blockType;
        uint64_t blockSize;
        uint64_t linkCount;
    };

}

#endif //MDFSIMPLECONVERTERS_MDFHEADER_H
