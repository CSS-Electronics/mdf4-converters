#ifndef MDFSORTER_MDF_HEADER_H
#define MDFSORTER_MDF_HEADER_H

#include "MDF_Type.h"

namespace mdf {

    struct MDF_Header {
        MDF_Type type;
        uint64_t size;
        uint64_t linkCount;
    };

}

#endif //MDFSORTER_MDF_HEADER_H
