#ifndef MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H
#define MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H

#include <map>
#include <vector>

#include "SDBlock.h"

namespace mdf {

  struct SDBlockDiscontinuous : public SDBlock {
    explicit SDBlockDiscontinuous(std::vector<uint8_t const *> const& dataPtr);

    uint8_t const *operator[](std::size_t index) override;
  protected:
    bool load(uint8_t const *dataPtr) override;
    bool saveBlockData(uint8_t *dataPtr) override;

    std::vector<uint8_t const *> dataRecords;
    std::map<uint8_t const *, std::size_t> dataRecordsMap;
  };

}

#endif //MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H
