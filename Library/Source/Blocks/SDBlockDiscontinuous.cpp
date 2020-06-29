#include <boost/endian.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include <streambuf>

#include "SDBlockDiscontinuous.h"

namespace be = boost::endian;

namespace mdf {

    SDBlockDiscontinuous::SDBlockDiscontinuous(std::vector<uint64_t> const &dataPtr, std::shared_ptr<std::streambuf> stream) : dataRecords(dataPtr), stream(stream) {
        // The data pointer has the indices of the start of each record. We want a map with the offset as the key and the
        // record as the value. Assumes data is sorted.

        // Pre-allocate.
        links.reserve(dataPtr.size());

        // Iterate over all records, to create a (address, data) map.
        uint64_t location = 0;

        for (auto entry: dataRecords | boost::adaptors::indexed(0)) {
            // Insert into map.
            dataRecordsMap.emplace(std::make_pair(location, entry.index()));

            // Determine where the next record would be.
            be::little_uint32_at vlsdLength = 0;
            stream->pubseekpos(entry.value());
            stream->sgetn(reinterpret_cast<char*>(vlsdLength.data()), sizeof(vlsdLength));

            location += 4;
            location += vlsdLength;
        }

        // Update the header block.
        header.blockSize += location;
    }

    std::size_t SDBlockDiscontinuous::operator[](std::size_t index) {
        auto ptr = static_cast<uint64_t>(index);
        auto iter = dataRecordsMap.find(ptr);

        if (iter == dataRecordsMap.end()) {
            throw std::runtime_error("Requested index is not in map");
        }

        std::size_t realIndex = iter->second;
        auto result = dataRecords[realIndex];

        return result;
    }

    bool SDBlockDiscontinuous::load(std::shared_ptr<std::streambuf> stream) {
        throw std::runtime_error("Can not load discontinuous SD block");
    }

#pragma pack(push, 1)
    struct SDRecord {
        boost::endian::little_uint32_buf_t recordSize;
        boost::endian::little_uint8_buf_t recordData[];
    };
#pragma pack(pop)

    bool SDBlockDiscontinuous::saveBlockData(std::streambuf *outputStream) {
        std::streamsize written = 0;

        // Create output iterator.
        std::ostreambuf_iterator<char> iterOut(outputStream);

        for (auto const &record: dataRecords) {
            // Re-seek the input stream, and determine how many bytes to read.
            stream->pubseekoff(record, std::ios_base::beg);
            boost::endian::little_uint32_buf_t recordSize;
            stream->sgetn(reinterpret_cast<char *>(recordSize.data()), sizeof(recordSize));

            // Write output.
            std::istreambuf_iterator<char> iterIn(stream.get());

            written += outputStream->sputn(reinterpret_cast<const char *>(recordSize.data()), sizeof(recordSize));
            std::copy_n(iterIn, recordSize.value(), iterOut);
            written += recordSize.value();
        }

        return true;
    }

}
