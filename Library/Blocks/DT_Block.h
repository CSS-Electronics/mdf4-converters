#ifndef MDFSORTER_DT_BLOCK_H
#define MDFSORTER_DT_BLOCK_H

#include "MDF_Block.h"

#include <cstdint>
#include <map>
#include <memory>
#include <set>

#include "GenericDataRecord.h"
#include "GenericDataRecordIterator.h"
#include "MDF_DataSource.h"

namespace mdf {

    struct DT_Block : public MDF_Block, public MDF_DataSource {
        // Constructors.
        DT_Block();
        DT_Block(DT_Block& value, uint64_t id);
        DT_Block(DT_Block& value, std::set<uint64_t> ids);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        // Setters.

        // Iterators.
        GenericDataRecordIterator begin();
        GenericDataRecordIterator end();

        void setupRecordInformation(std::map<uint64_t, int64_t> values, uint64_t idLength);
        std::shared_ptr<GenericDataRecord> getNextRecord(std::istream& stream);
        std::shared_ptr<GenericDataRecord> getNextRecord(std::istream& stream, std::set<uint64_t> const& ids);
        std::shared_ptr<GenericDataRecord> getNextRecord(std::istream& stream, uint64_t const& id);

        std::map<uint64_t, int64_t> recordInformation;
        std::set<uint64_t> recordIDs;
        uint64_t recordIdBytes;
        uint64_t currentLocation;

        std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> getRecord(std::size_t index) override;
        std::streambuf* handle;
        uint64_t handleDataLocation;
    private:
        std::shared_ptr<DT_Block> dataSource;
    };

}

#endif //MDFSORTER_DT_BLOCK_H
