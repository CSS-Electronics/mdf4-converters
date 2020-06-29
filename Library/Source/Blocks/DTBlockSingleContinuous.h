#ifndef MDFSIMPLECONVERTERS_DTBLOCKSINGLECONTINUOUS_H
#define MDFSIMPLECONVERTERS_DTBLOCKSINGLECONTINUOUS_H

#include "DTBlock.h"

namespace mdf {

  struct DTBlockSingleContinuous : DTBlock {
    DTBlockSingleContinuous(DTBlock const& parent, std::size_t recordSize);

    uint64_t operator[](std::size_t index) override;

  protected:
    bool saveBlockData(uint8_t * dataPtr) override;
    std::size_t recordSize;
  };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCKSINGLECONTINUOUS_H
