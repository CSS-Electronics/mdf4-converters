#ifndef MDFSIMPLECONVERTERS_DTBLOCKMULTIPLERECORDIDS_H
#define MDFSIMPLECONVERTERS_DTBLOCKMULTIPLERECORDIDS_H

#include <map>

#include "DTBlock.h"

namespace mdf {

  struct DTBlockMultipleRecordIDs : DTBlock {
    explicit DTBlockMultipleRecordIDs(DTBlock const &parent);

    DTBlockMultipleRecordIDs(DTBlockMultipleRecordIDs const &parent, std::vector<uint64_t> IDs);

    void setRecordInformation(std::map<uint64_t, int64_t> recordSizes, uint64_t recordLength);

    void index();

    long long findFirstMatching(std::vector<uint64_t> targetIDs);

    std::size_t operator[](std::size_t index) override;

    [[nodiscard]] std::map<uint64_t, int64_t> getRecordCycleCounts() const;

    [[nodiscard]] std::vector<uint64_t> getRecordIndicesAbsolute(uint64_t recordID) const;

    [[nodiscard]] std::vector<uint64_t> getRegisteredRecordIDs() const;

    [[nodiscard]] uint64_t getTotalSize(uint64_t recordID) const;

  protected:
    bool saveBlockData(uint8_t *dataPtr) override;

    /**
     * A lookup table containing each record offset in this block. The record ID is the key, while
     * a vector of offsets is the value.
     */
    std::map<uint64_t, std::vector<uint64_t>> recordIndices;

    /**
     * A map of the total number of bytes in each record ID.
     */
    std::map<uint64_t, uint64_t> recordSizes;

    /**
     * Map over records IDs (key) and sizes. For VLSD records, this size is set to -1.
     */
    std::map<uint64_t, int64_t> recordSizeMap;

    /**
     * How many bytes are used for each record in this DT block.
     */
    uint64_t recordLength;
  };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCKMULTIPLERECORDIDS_H
