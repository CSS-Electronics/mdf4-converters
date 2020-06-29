#ifndef MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H
#define MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H

#include "SDBlock.h"

namespace mdf {

    struct SDBlockContinuous : public SDBlock {
        std::size_t operator[](std::size_t index) override;

    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;

        bool saveBlockData(std::streambuf *outputDataPtr) override;

        std::shared_ptr<std::streambuf> stream;
    };

}

#endif //MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H
