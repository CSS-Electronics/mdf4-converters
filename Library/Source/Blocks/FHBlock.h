#ifndef MDFSIMPLECONVERTERS_FHBLOCK_H
#define MDFSIMPLECONVERTERS_FHBLOCK_H

#include "MdfBlock.h"

namespace mdf {

    struct FHBlock : MdfBlock {
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(uint8_t * dataPtr) override;
    private:
        uint64_t timeNs;
        int16_t tzOffsetMin;
        int16_t dstOffsetMin;
        uint8_t timeFlags;
    };

}

#endif //MDFSIMPLECONVERTERS_FHBLOCK_H
