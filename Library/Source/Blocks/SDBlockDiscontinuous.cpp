#include <boost/endian/buffers.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include "SDBlockDiscontinuous.h"

namespace mdf {

  SDBlockDiscontinuous::SDBlockDiscontinuous(std::vector<uint8_t const *> const& dataPtr) : dataRecords(dataPtr)
  {
    // The data pointer has the indices of the start of each record. We want a map with the offset as the key and the
    // record as the value. Assumes data is sorted.

    // Pre-allocate.
    links.reserve(dataPtr.size());

    // Iterate over all records, to create a (address, data) map.
    uint8_t* ptr = 0;

    for(auto entry: dataRecords | boost::adaptors::indexed(0)) {
      // Insert into map.
      dataRecordsMap.emplace(std::make_pair(ptr, entry.index()));

      // Determine where the next record would be.
      auto lengthAddress = reinterpret_cast<uint32_t const*>(entry.value());
      ptr += 4;
      ptr += *lengthAddress;
    }

    // Update the header block.
    header.blockSize += (ptr - static_cast<uint8_t*>(0));
  }

  const uint8_t *SDBlockDiscontinuous::operator[](std::size_t index) {
    auto ptr = reinterpret_cast<uint8_t const*>(index);
    auto iter = dataRecordsMap.find(ptr);

    if(iter == dataRecordsMap.end()) {
      throw std::runtime_error("Requested index is not in map");
    }

    std::size_t realIndex = iter->second;

    return dataRecords[realIndex];
  }

  bool SDBlockDiscontinuous::load(uint8_t const *inputDataPtr) {
    throw std::runtime_error("Can not load discontinuous SD block");
  }

#pragma pack(push, 1)
  struct SDRecord {
    boost::endian::little_uint32_buf_t recordSize;
    boost::endian::little_uint8_buf_t recordData[];
  };
#pragma pack(pop)

  bool SDBlockDiscontinuous::saveBlockData(uint8_t *dataPtr) {
    for (auto const &record: dataRecords) {
      // Determine how many bytes to read.
      auto input = reinterpret_cast<SDRecord const *>(record);

      auto output = reinterpret_cast<SDRecord *>(dataPtr);
      output->recordSize = input->recordSize;

      std::copy(input->recordData, input->recordData + input->recordSize.value(), output->recordData);
      dataPtr += sizeof(SDRecord) + input->recordSize.value();
    }

    return true;
  }

}
