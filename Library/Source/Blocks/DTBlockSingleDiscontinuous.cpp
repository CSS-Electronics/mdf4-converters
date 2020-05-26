#include "DTBlockSingleDiscontinuous.h"

#include <algorithm>
#include <stdexcept>

namespace mdf {

  constexpr MdfHeader DTBlockHeader = {
    .blockType = MdfBlockType_DT,
    .blockSize = 24,
    .linkCount = 0
  };

  DTBlockSingleDiscontinuous::DTBlockSingleDiscontinuous(DTBlock const &parent) : DTBlock(parent) {

  }

  DTBlockSingleDiscontinuous::DTBlockSingleDiscontinuous(const DTBlockMultipleRecordIDs &parent, uint64_t recordID)
    : DTBlock(parent) {
    // Reset header.
    header = DTBlockHeader;

    auto size = parent.getTotalSize(recordID);

    header.blockSize += size;
    recordIndices = parent.getRecordIndicesAbsolute(recordID);

    if (!recordIndices.empty()) {
      recordSize = size / recordIndices.size();

      if (size % recordIndices.size() != 0) {
        throw std::runtime_error("Not constant number of bytes in record");
      }
    } else {
      recordSize = 0;
    }
  }

  bool DTBlockSingleDiscontinuous::saveBlockData(uint8_t *dataPtr) {
    // Since only a single type of record is present, simply write all the records.
    // NOTE: While this should be a simple copy from the start of the data ptr and size bytes forward, the
    //       implementation uses the vector with indices, since the source may be a DT block with multiple IDs,
    //       such that data are not stored sequentially for a single ID.
    for (auto const &record: recordIndices) {
      std::copy_n(record, recordSize, dataPtr);
      dataPtr += recordSize;
    }

    return true;
  }

  std::vector<uint8_t const *> const &DTBlockSingleDiscontinuous::getRecordIndicesAbsolute() const {
    return recordIndices;
  }

  uint8_t const *DTBlockSingleDiscontinuous::operator[](std::size_t index) {
    return recordIndices[index];
  }

}
