#include "DTBlockMultipleRecordIDs.h"

#include <algorithm>
#include <iterator>
#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    constexpr MdfHeader DTBlockHeader(MdfBlockType_DT, 24, 0);

#pragma pack(push, 1)
    struct SDRecord {
        be::little_uint32_buf_t recordSize;
        be::little_uint8_buf_t recordData[];
    };
#pragma pack(pop)

    DTBlockMultipleRecordIDs::DTBlockMultipleRecordIDs(DTBlock const &parent) : DTBlock(parent) {

    }

    DTBlockMultipleRecordIDs::DTBlockMultipleRecordIDs(DTBlockMultipleRecordIDs const &parent,
                                                       std::vector<uint64_t> IDs) : DTBlock(parent) {
        // Reset header.
        header = DTBlockHeader;

        // Copy over the maps.
        for (auto &recordID: IDs) {
            recordIndices.insert(std::make_pair(recordID, parent.recordIndices.at(recordID)));
            recordSizes.insert(std::make_pair(recordID, parent.recordSizes.at(recordID)));
            recordSizeMap.insert(std::make_pair(recordID, parent.recordSizeMap.at(recordID)));

            header.blockSize += parent.recordSizes.at(recordID);

            if (parent.recordSizeMap.at(recordID) == -1) {
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

    std::vector<uint64_t> DTBlockMultipleRecordIDs::getRecordIndicesAbsolute(uint64_t recordID) const {
        std::vector<uint64_t> result;

        auto iter = recordIndices.find(recordID);
        if (iter != std::end(recordIndices)) {
            result.reserve(iter->second.size());

            for (auto const &record: iter->second) {
                result.emplace_back(record);
            }
        }

        return result;
    }

    std::vector<uint64_t> DTBlockMultipleRecordIDs::getRegisteredRecordIDs() const {
        std::vector<uint64_t> result;

        for (auto const &recordID: recordSizes) {
            result.emplace_back(recordID.first);
        }

        return result;
    }

    uint64_t DTBlockMultipleRecordIDs::getTotalSize(uint64_t recordID) const {
        uint64_t result = 0;

        auto iter = recordSizes.find(recordID);
        if (iter != std::end(recordSizes)) {
            result = iter->second;
        }

        return result;
    }

    long long DTBlockMultipleRecordIDs::findFirstMatching(std::vector<uint64_t> targetIDs) {
        long long result = -1;

        std::size_t currentLocation = stream->pubseekoff(rawFileLocation + 24, std::ios_base::beg);
        std::size_t endLocation = currentLocation + header.blockSize - sizeof(header);

        while (currentLocation < endLocation) {
            stream->pubseekoff(currentLocation, std::ios_base::beg);

            // Read the record id.
            uint64_t recordID = 0;
            stream->sgetn(reinterpret_cast<char *>(&recordID), recordLength);

            // Stop iterating if this is a match.
            if (std::find(targetIDs.begin(), targetIDs.end(), recordID) != targetIDs.end()) {
                result = currentLocation;
                break;
            }

            currentLocation += recordLength;

            // Determine how many bytes to jump forward.
            int64_t recordSize = recordSizeMap.at(recordID);

            if (recordSize < 0) {
                // VLSD record, read the next 4 bytes instead as record size.
                uint32_t VLSDrecordSize = 0;
                stream->sgetn(reinterpret_cast<char *>(&VLSDrecordSize), sizeof(VLSDrecordSize));
                currentLocation += sizeof(VLSDrecordSize);
                recordSize = VLSDrecordSize;
            }

            // Update record size and jump to next record.
            recordSizes.at(recordID) += recordSize;

            currentLocation += recordSize;
        }

        return result;
    }

    void DTBlockMultipleRecordIDs::index() {
        // Clear any previous indexes.
        recordIndices.clear();

        // Create a index entry for each.
        for (auto &entry: recordSizeMap) {
            recordIndices.insert(std::make_pair(entry.first, std::vector<uint64_t>()));
            recordSizes.insert(std::make_pair(entry.first, 0));
        }

        // Iterate over all the records, saving indices for quick access.
        std::streampos const blockDataStart = getFileLocation();
        std::streampos const blockDataEnd = getFileLocation() + header.blockSize - sizeof(header);
        std::streampos currentLocation = blockDataStart;

        while (currentLocation < blockDataEnd) {
            // Read the record id.
            stream->pubseekpos(currentLocation);
            //be::little_uint64_at recordID = 0;
            // TODO: Investigate endian
            uint64_t recordID = 0;

            std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char *>(&recordID), recordLength);

            if (bytesRead != recordLength) {
                throw std::runtime_error("Could not read record ID");
            }

            currentLocation += recordLength;

            // Store this in the index.
            recordIndices.at(recordID).emplace_back(currentLocation);

            // Determine how many bytes to jump forward.
            int64_t recordSize = recordSizeMap.at(recordID);

            if (recordSize < 0) {
                // VLSD record, read the next 4 bytes instead as record size.
                be::little_uint32_at vlsdSize;

                bytesRead = stream->sgetn(reinterpret_cast<char *>(vlsdSize.data()), 4);
                if (bytesRead != 4) {
                    throw std::runtime_error("Could not read VLSD length");
                }
                recordSize = vlsdSize;

                currentLocation += 4;
            }

            // Update record size and jump to next record.
            recordSizes.at(recordID) += recordSize;

            currentLocation += recordSize;
        }
    }

    void
    DTBlockMultipleRecordIDs::setRecordInformation(std::map<uint64_t, int64_t> recordSizes, uint64_t recordLength) {
        this->recordSizeMap = std::move(recordSizes);
        this->recordLength = recordLength;
    }

    std::size_t DTBlockMultipleRecordIDs::operator[](std::size_t index) {
        return 0;
    }

    bool DTBlockMultipleRecordIDs::saveBlockData(std::streambuf *outputStream) {
        std::streamsize written = 0;

        // Create output iterator.
        std::ostreambuf_iterator<char> iterOut(outputStream);

        for(auto const& recordInformation: recordIndices) {
            uint64_t recordSize = recordSizeMap.at(recordInformation.first);

            if(recordSize < 0) {
                /*for(auto const& sourceAddress: recordInformation.second) {
                    // Re-seek the input stream, and determine how many bytes to read.
                    stream->pubseekoff(sourceAddress, std::ios_base::beg);
                    boost::endian::little_uint32_buf_t recordSizeData;
                    stream->sgetn(reinterpret_cast<char *>(recordSizeData.data()), sizeof(recordSizeData));

                    auto input = reinterpret_cast<SDRecord const*>(sourceAddress);

                    auto output = reinterpret_cast<SDRecord*>(dataPtr);
                    output->recordSize = input->recordSize;

                    std::copy(input->recordData, input->recordData + input->recordSize.value(), output->recordData);
                    dataPtr += sizeof(SDRecord) + input->recordSize.value();
                }*/
            } else {
                for(auto const& sourceAddress: recordInformation.second) {
                    stream->pubseekoff(sourceAddress, std::ios_base::beg);
                    std::istreambuf_iterator<char> iterIn(stream.get());

                    std::copy_n(iterIn, recordSize, iterOut);
                    written += recordSize;
                }
            }

        }

        return true;
    }

}
