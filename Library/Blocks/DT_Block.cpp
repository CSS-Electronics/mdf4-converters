#include "DT_Block.h"

#include <algorithm>
#include <iterator>

namespace mdf {

    static uint64_t constexpr size = 24;
    static uint64_t constexpr linkCount = 0;

    DT_Block::DT_Block() {
        // Initialize header.
        header.type = MDF_Type_DT;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;

        // Others.
        handle = nullptr;
        handleDataLocation = 0;
        recordIdBytes = 0;
        currentLocation = 0;
    }

    DT_Block::DT_Block(mdf::DT_Block &value, uint64_t id) : DT_Block() {
        // Copy over the location of the data.
        handle = value.handle;
        handleDataLocation = value.handleDataLocation;
        recordIdBytes = value.recordIdBytes;

        recordIDs.insert(id);
        recordInformation.emplace(id, value.recordInformation.at(id));

        dataSource = std::dynamic_pointer_cast<DT_Block>(value.shared_from_this());
    }

    DT_Block::DT_Block(mdf::DT_Block &value, std::set<uint64_t> ids) : DT_Block() {
        // Copy over the location of the data.
        handle = value.handle;
        handleDataLocation = value.handleDataLocation;
        recordIdBytes = value.recordIdBytes;

        for(auto id: ids) {
            recordIDs.insert(id);
            recordInformation.emplace(id, value.recordInformation.at(id));
        }

        dataSource = std::dynamic_pointer_cast<DT_Block>(value.shared_from_this());
    }

    bool DT_Block::load(std::istream &stream) {
        // Let the base handle the initial loading.
        bool result = MDF_Block::load(stream);

        if(!result) {
            return result;
        }

        // There are no links in a DT block.

        // Create a stream to the same buffer.
        handle = stream.rdbuf();
        handleDataLocation = stream.tellg();

        return !stream.fail();
    }

    void DT_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);
        stream.flush();

        // No links to save.

        // Save the data by copying it from the input data source, record by record.
        std::istream inStream(handle);
        inStream.seekg(handleDataLocation);
        currentLocation = 0;

        std::shared_ptr<GenericDataRecord> currentRecord = getNextRecord(inStream, recordIDs);

        while(currentRecord) {
            // If unsorted, write ID.
            if(recordIDs.size() > 1) {
                // Write record ID.
                const char *dataPtr = reinterpret_cast<const char *>(&currentRecord->recordID);
                stream.write(dataPtr, recordIdBytes);

                // If VLSD, write record length.
                int64_t dataLength = recordInformation[currentRecord->recordID];
                if (dataLength < 0) {
                    dataLength = currentRecord->data.size();
                    dataPtr = reinterpret_cast<const char *>(&dataLength);
                    stream.write(dataPtr, 4);
                }
            }

            // Write record data.
            std::copy(currentRecord->data.cbegin(), currentRecord->data.cend(), std::ostreambuf_iterator<char>(stream));

            // Get next record.
            currentRecord = getNextRecord(inStream, recordIDs);
        }

        // Handle any alignment issues.
        auto alignmentError = (header.size - sizeof(MDF_Header)) % 8;

        if(alignmentError != 0) {
            uint64_t location = stream.tellp();
            location += 8 - alignmentError;
            stream.seekp(location);
        }
    }

    std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> DT_Block::getRecord(std::size_t index) {
        // Setup a stream to the correct location.
        std::istream inStream(handle);
        inStream.seekg(handleDataLocation + index);
        currentLocation = index;

        std::shared_ptr<GenericDataRecord> value = getNextRecord(inStream, recordIDs);
        std::size_t nextLocation = inStream.tellg();
        nextLocation -= handleDataLocation;

        return std::make_pair(value, nextLocation);
    }

    GenericDataRecordIterator DT_Block::begin() {
        // Create a record iterator pointing to the first object.
        GenericDataRecordIterator iter(*this);

        return iter;
    }

    GenericDataRecordIterator DT_Block::end() {
        GenericDataRecordIterator iter(*this, true);

        return iter;
    }

    void DT_Block::setupRecordInformation(std::map<uint64_t, int64_t> values, uint64_t idLength) {
        recordInformation = values;
        recordIdBytes = idLength;

        for(auto const& entry: values) {
            recordIDs.insert(entry.first);
        }
    }

    std::shared_ptr<GenericDataRecord> DT_Block::getNextRecord(std::istream& stream) {
        std::shared_ptr<GenericDataRecord> result;

        // If a data source is configured, use that instead.
        if(dataSource) {
            auto res = dataSource->getRecord(currentLocation);
            result = res.first;
            currentLocation = res.second;
        } else {
            if(currentLocation >= header.size - sizeof(header) - 1) {
                return result;
            }

            result = std::make_shared<GenericDataRecord>();

            // If only one ID is known, all fields have a fixed size of that id.
            int64_t dataLength = 0;
            if(recordInformation.size() == 1) {
                dataLength = recordInformation.cbegin()->second;
            } else {
                // Read the ID field.
                char* idPtr = reinterpret_cast<char*>(&result->recordID);
                stream.read(idPtr, recordIdBytes);
                currentLocation += recordIdBytes;

                // Lookup the record.
                try {
                    dataLength = recordInformation.at(result->recordID);
                } catch (std::out_of_range const& e) {
                    std::cerr << "Invalid record ID: " << result->recordID << ", at " << stream.tellg() << std::endl;
                }
            }

            if(dataLength < 0) {
                // VLSD, read the next 4 bytes to get VLSD length.
                dataLength = 0;
                char* dataLengthPtr = reinterpret_cast<char*>(&dataLength);
                stream.read(dataLengthPtr, 4);
                currentLocation += 4;
            }

            // Copy over the data (if any).
            if(dataLength > 0) {
                std::copy_n(std::istreambuf_iterator<char>(stream), dataLength, std::back_insert_iterator<std::vector<uint8_t>>(result->data));
                currentLocation += dataLength;

                // TODO: For some reason it is necessary to increment the stream by one?
                uint64_t loc = stream.tellg();
                stream.seekg(loc + 1);
            }
        }

        return result;
    }

    std::shared_ptr<GenericDataRecord> DT_Block::getNextRecord(std::istream &stream, std::set<uint64_t> const& ids) {
        // Get the next record.
        std::shared_ptr<GenericDataRecord> result = getNextRecord(stream);

        if(!ids.empty()) {
            while (result) {
                if (ids.find(result->recordID) != ids.cend()) {
                    break;
                }

                result = getNextRecord(stream);
            }
        }

        return result;
    }

    std::shared_ptr<GenericDataRecord> DT_Block::getNextRecord(std::istream &stream, uint64_t const& id) {
        std::set<uint64_t> ids;
        ids.insert(id);

        // Get the next record.
        std::shared_ptr<GenericDataRecord> result = getNextRecord(stream, ids);

        return result;
    }

}
