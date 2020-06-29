#ifndef MDFSIMPLECONVERTERS_DTBLOCKRAW_H
#define MDFSIMPLECONVERTERS_DTBLOCKRAW_H

#include "DTBlock.h"

namespace mdf {

    struct DTBlockRaw : DTBlock {
        std::size_t operator[](std::size_t index) override;

    protected:
        bool saveBlockData(std::streambuf *stream) override;
    };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCKRAW_H
