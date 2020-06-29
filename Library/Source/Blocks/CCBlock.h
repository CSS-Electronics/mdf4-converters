#ifndef MDFSIMPLECONVERTERS_CCBLOCK_H
#define MDFSIMPLECONVERTERS_CCBLOCK_H

#include "MdfBlock.h"

#include <vector>

namespace mdf {

    struct CCBlock : MdfBlock {
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(uint8_t * dataPtr) override;
    private:
        uint8_t type;
        uint8_t precision;
        uint16_t flags;
        uint16_t refCount;
        uint16_t valueCount;
        double physicalRangeMinimum;
        double physicalRangeMaximum;

        std::vector<double> convertionValues;
    };

}

#endif //MDFSIMPLECONVERTERS_CCBLOCK_H
