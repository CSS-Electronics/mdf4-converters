#ifndef MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H
#define MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H

#include "SDBlock.h"

namespace mdf {

  struct SDBlockContinuous : public SDBlock {
    uint8_t const *operator[](std::size_t index) override;
  protected:
    bool load(uint8_t const *dataPtr) override;
    bool saveBlockData(uint8_t *outputDataPtr) override;

    uint8_t const* dataPtr;
  };

}


#endif //MDFSIMPLECONVERTERS_SDBLOCKCONTINUOUS_H
