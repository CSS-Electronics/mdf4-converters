#include "SD_Block.h"

#include "CG_Block.h"
#include "DT_Block.h"

#include "MDF_BlockSizeError.h"
#include "MDF_MissingBlockError.h"
#include "MDF_NotImplementedError.h"

namespace mdf {

    static uint64_t constexpr size = sizeof(MDF_Header);
    static uint64_t constexpr linkCount = 0;

    SD_Block::SD_Block() {
        // Initialize header.
        header.type = MDF_Type_SD;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;

        lastIndex = 0;
        lastMappedIndex = 0;
    }

    SD_Block::SD_Block(std::shared_ptr<mdf::DT_Block> dtBlock, std::shared_ptr<mdf::CG_Block> cgBlock) : SD_Block() {
        // Save a handle to the data source.
        dataSource = dtBlock;

        // Save a handle to the correct data group.
        recordID = cgBlock->data.record_id;

        // Set the block size from the CG block.
        header.size = sizeof(MDF_Header) + cgBlock->data.data_bytes + cgBlock->data.cycle_count * 4;
    }

    bool SD_Block::load(std::istream &stream) {
        throw MDF_NotImplementedError("Load not supported for SD blocks.");
    }

    void SD_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        uint64_t locc1 = stream.tellp();
        MDF_Block::save(stream);
        stream.flush();

        uint64_t locc2 = stream.tellp();

        // No links to save.

        // If no datasource can be found, raise an error.
        if(!dataSource) {
            throw MDF_MissingBlockError::Create(shared_from_this(), "dataSource");
        }

        // Save the data by copying it from the input data source, record by record.
        std::istream inStream(dataSource->handle);
        inStream.seekg(dataSource->handleDataLocation);
        dataSource->currentLocation = 0;

        std::shared_ptr<GenericDataRecord> currentRecord = dataSource->getNextRecord(inStream, recordID);
        int i = 0;

        while(currentRecord) {
            // Write record length.
            int64_t dataLength = currentRecord->data.size();
            const char *dataPtr = reinterpret_cast<const char *>(&dataLength);
            stream.write(dataPtr, 4);

            // Write the data.
            std::copy(currentRecord->data.cbegin(), currentRecord->data.cend(), std::ostreambuf_iterator<char>(stream));

            // Get next record.
            currentRecord = dataSource->getNextRecord(inStream, recordID);
            i++;
        }

        // Sanity check.
        std::size_t currentLocation = stream.tellp();
        int64_t difference = currentLocation - fileLocation;

        if(difference != header.size) {
            throw MDF_BlockSizeError::Create(shared_from_this(), static_cast<uint64_t>(currentLocation));
        }

        // Handle any alignment issues.
        auto alignmentError = (header.size - sizeof(MDF_Header)) % 8;

        if(alignmentError != 0) {
            uint64_t location = stream.tellp();
            location += 8 - alignmentError;
            stream.seekp(location);
        }
    }

    std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> SD_Block::getRecord(std::size_t index) {
        std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> result;

        // Is the index before our last mapped index?
        if(index < lastMappedIndex) {
            // Start from the beginning.
            lastIndex = 0;
        }

        // Request from the underlying data source with the next index.
        result = dataSource->getRecord(lastIndex);

        lastMappedIndex = index;
        lastIndex = result.second;

        return result;
    }

}
