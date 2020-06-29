#ifndef MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H
#define MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H

#include "DTBlock.h"
#include "DTBlockMultipleRecordIDs.h"

namespace mdf {

    struct DTBlockSingleDiscontinuous : DTBlock {
        explicit DTBlockSingleDiscontinuous(DTBlock const &parent);

        DTBlockSingleDiscontinuous(DTBlockMultipleRecordIDs const &parent, uint64_t recordID);

        std::vector<uint64_t> const &getRecordIndicesAbsolute() const;

        std::size_t operator[](std::size_t index) override;

    protected:
        bool saveBlockData(std::streambuf *stream) override;

        std::vector<uint64_t> recordIndices;
        uint64_t recordSize;
    };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCKSINGLEDISCONTINUOUS_H
