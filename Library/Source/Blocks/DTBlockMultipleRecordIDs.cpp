#include "DTBlockMultipleRecordIDs.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

    constexpr MdfHeader DTBlockHeader = {
        .blockType = MdfBlockType_DT,
        .blockSize = 24,
        .linkCount = 0
    };

    #pragma pack(push, 1)
    struct SDRecord {
        boost::endian::little_uint32_buf_t recordSize;
        boost::endian::little_uint8_buf_t recordData[];
    };
    #pragma pack(pop)

    DTBlockMultipleRecordIDs::DTBlockMultipleRecordIDs(DTBlock const& parent) : DTBlock(parent) {

    }

    DTBlockMultipleRecordIDs::DTBlockMultipleRecordIDs(DTBlockMultipleRecordIDs const& parent, std::vector<uint64_t> IDs) : DTBlock(parent) {
        // Reset header.
        header = DTBlockHeader;

        // Copy over the maps.
        for (auto &recordID: IDs) {
            recordIndices.insert(std::make_pair(recordID, parent.recordIndices.at(recordID)));
            recordSizes.insert(std::make_pair(recordID, parent.recordSizes.at(recordID)));
            recordSizeMap.insert(std::make_pair(recordID, parent.recordSizeMap.at(recordID)));

            header.blockSize += parent.recordSizes.at(recordID);

            if(parent.recordSizeMap.at(recordID) == -1) {
                header.blockSize += 4 * parent.recordIndices.at(recordID).size();
            }
        }
    }

    std::map<uint64_t, int64_t> DTBlockMultipleRecordIDs::getRecordCycleCounts() const {
        std::map<uint64_t, int64_t> result;

        for (auto const &value: recordIndices) {
            result.insert(std::make_pair(value.first, value.second.size()));
        }

        return result;
    }

    std::vector<uint8_t const*> DTBlockMultipleRecordIDs::getRecordIndicesAbsolute(uint64_t recordID) const {
        std::vector<uint8_t const*> result;

        auto iter = recordIndices.find(recordID);
        if(iter != std::end(recordIndices)) {
            result.reserve(iter->second.size());

            for(auto const& record: iter->second) {
                result.emplace_back(record);
            }
        }

        return result;
    }

    std::vector<uint64_t> DTBlockMultipleRecordIDs::getRegisteredRecordIDs() const {
        std::vector<uint64_t> result;

        for(auto const& recordID: recordSizes) {
            result.emplace_back(recordID.first);
        }

        return result;
    }

    uint64_t DTBlockMultipleRecordIDs::getTotalSize(uint64_t recordID) const {
        uint64_t result = 0;

        auto iter = recordSizes.find(recordID);
        if(iter != std::end(recordSizes)) {
            result = iter->second;
        }

        return result;
    }

    void DTBlockMultipleRecordIDs::index() {
        // Clear any previous indexes.
        recordIndices.clear();

        // Create a index entry for each.
        for (auto &entry: recordSizeMap) {
            recordIndices.insert(std::make_pair(entry.first, std::vector<uint8_t const*>()));
            recordSizes.insert(std::make_pair(entry.first, 0));
        }

        // Iterate over all the records, saving indices for quick access.
        uint8_t const* currentPtr = dataPtr;
        uint8_t const* const endPtr = dataPtr + header.blockSize - sizeof(header);

        while (currentPtr < endPtr) {
            // Read the record id.
            uint64_t recordID = 0;
            memcpy(&recordID, currentPtr, recordLength);
            currentPtr += recordLength;

            // Store this in the index.
            recordIndices.at(recordID).emplace_back(currentPtr);

            // Determine how many bytes to jump forward.
            int64_t recordSize = recordSizeMap.at(recordID);

            if (recordSize < 0) {
                // VLSD record, read the next 4 bytes instead as record size.
                auto ptr = reinterpret_cast<uint32_t const*>(currentPtr);
                recordSize = *ptr;

                currentPtr += 4;
            }

            // Update record size and jump to next record.
            recordSizes.at(recordID) += recordSize;

            currentPtr += recordSize;
        }
    }

    bool DTBlockMultipleRecordIDs::saveBlockData(uint8_t* dataPtr) {
        // TODO: Does not handle record IDs.
        for(auto const& recordInformation: recordIndices) {
            uint64_t recordSize = recordSizeMap.at(recordInformation.first);

            if(recordSize < 0) {
                for(auto const& sourceAddress: recordInformation.second) {
                    // Determine how many bytes to read.
                    auto input = reinterpret_cast<SDRecord const*>(sourceAddress);

                    auto output = reinterpret_cast<SDRecord*>(dataPtr);
                    output->recordSize = input->recordSize;

                    std::copy(input->recordData, input->recordData + input->recordSize.value(), output->recordData);
                    dataPtr += sizeof(SDRecord) + input->recordSize.value();
                }
            } else {
                for(auto const& sourceAddress: recordInformation.second) {
                    std::copy(sourceAddress, sourceAddress + recordSize, dataPtr);
                    dataPtr += recordSize;
                }
            }

        }

        return true;
    }

    void DTBlockMultipleRecordIDs::setRecordInformation(std::map<uint64_t, int64_t> recordSizes, uint64_t recordLength) {
        this->recordSizeMap = std::move(recordSizes);
        this->recordLength = recordLength;
    }

  uint8_t const *DTBlockMultipleRecordIDs::operator[](std::size_t index) {
    return nullptr;
  }

}
