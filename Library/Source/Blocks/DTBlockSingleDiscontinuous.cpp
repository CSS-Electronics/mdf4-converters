#include "DTBlockSingleDiscontinuous.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <streambuf>

namespace mdf {

    constexpr MdfHeader DTBlockHeader(MdfBlockType_DT, 24, 0);

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

    bool DTBlockSingleDiscontinuous::saveBlockData(std::streambuf *outputStream) {
        // Since only a single type of record is present, simply write all the records.
        // NOTE: While this should be a simple copy from the start of the data ptr and size bytes forward, the
        //       implementation uses the vector with indices, since the source may be a DT block with multiple IDs,
        //       such that data are not stored sequentially for a single ID.
        std::streamsize written = 0;

        // Create output iterator.
        std::ostreambuf_iterator<char> iterOut(outputStream);

        for (auto const &record: recordIndices) {
            // Re-seek the input stream.
            stream->pubseekoff(record, std::ios_base::beg);
            std::istreambuf_iterator<char> iterIn(stream.get());

            // Copy over the record.
            std::copy_n(iterIn, recordSize, iterOut);
            written += recordSize;
        }

        return true;
    }

    std::vector<uint64_t> const &DTBlockSingleDiscontinuous::getRecordIndicesAbsolute() const {
        return recordIndices;
    }

    std::size_t DTBlockSingleDiscontinuous::operator[](std::size_t index) {
        return recordIndices[index];
    }

}
