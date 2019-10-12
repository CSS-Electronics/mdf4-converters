#include "GenericDataRecordIterator.h"

namespace mdf {

    GenericDataRecordIterator::GenericDataRecordIterator(MDF_DataSource& dataSource, bool end) : dataSource(dataSource) {
        if(!end) {
            auto res = dataSource.getRecord(0);

            value = res.first;
            index = res.second;
        }
    }

    bool GenericDataRecordIterator::operator==(const mdf::GenericDataRecordIterator &other) const {
        // Check if any of these are a null pointer.
        bool anyNulls = !value || !other.value;
        bool result = false;

        if(anyNulls) {
            result = !value && !other.value;
        } else {
            result = value == other.value;
        }

        return result;
    }

    bool GenericDataRecordIterator::operator!=(const mdf::GenericDataRecordIterator &other) const {
        return !(*this == other);
    }

    std::shared_ptr<GenericDataRecord> GenericDataRecordIterator::operator*() const {
        return value;
    }

    GenericDataRecordIterator& GenericDataRecordIterator::operator++() {
        // Get the next item.
        auto res = dataSource.getRecord(index);

        value = res.first;
        index = res.second;

        return *this;
    }

    GenericDataRecordIterator GenericDataRecordIterator::operator++(int) {
        // Create a copy of this object.
        GenericDataRecordIterator returnValue = *this;

        // Increment and return copy.
        ++*this;
        return returnValue;
    }

}
