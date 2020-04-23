#ifndef MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H
#define MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H

#include "DTBlock.h"
#include "DTBlockMultipleRecordIDs.h"

namespace mdf {

  struct DTBlockSingleDiscontinuous : DTBlock {
    explicit DTBlockSingleDiscontinuous(DTBlock const &parent);

    DTBlockSingleDiscontinuous(DTBlockMultipleRecordIDs const &parent, uint64_t recordID);

    [[nodiscard]] std::vector<uint8_t const *> const &getRecordIndicesAbsolute() const;

    uint8_t const *operator[](std::size_t index) override;

  protected:
    bool saveBlockData(uint8_t *dataPtr) override;

    std::vector<uint8_t const *> recordIndices;
    uint64_t recordSize;
  };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H
