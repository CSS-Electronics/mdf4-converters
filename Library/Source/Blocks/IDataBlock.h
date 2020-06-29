#ifndef MDFSIMPLECONVERTERS_IDATABLOCK_H
#define MDFSIMPLECONVERTERS_IDATABLOCK_H

#include <cstdint>

namespace mdf {

    struct IDataBlock {
        virtual std::size_t operator[](std::size_t index) = 0;
    };

}

#endif //MDFSIMPLECONVERTERS_IDATABLOCK_H
