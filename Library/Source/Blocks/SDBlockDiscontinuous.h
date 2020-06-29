#ifndef MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H
#define MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H

#include <map>
#include <vector>

#include "SDBlock.h"

namespace mdf {

  struct SDBlockDiscontinuous : public SDBlock {
    SDBlockDiscontinuous(std::vector<uint64_t> const& dataPtr, std::shared_ptr<std::streambuf> stream);

    uint64_t operator[](std::size_t index) override;
  protected:
    bool load(std::shared_ptr<std::streambuf> stream) override;
    bool saveBlockData(uint8_t *dataPtr) override;

    std::vector<uint64_t> dataRecords;
    std::map<uint64_t, std::size_t> dataRecordsMap;
    std::shared_ptr<std::streambuf> stream;
  };

}

#endif //MDFSIMPLECONVERTERS_SDBLOCKDISCONTINUOUS_H
