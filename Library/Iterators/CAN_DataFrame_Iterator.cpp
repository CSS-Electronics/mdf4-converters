#include "CAN_DataFrame_Iterator.h"

namespace mdf {

    CAN_DataFrame_Iterator::CAN_DataFrame_Iterator() {
        //
    }

    CAN_DataFrame_Iterator::CAN_DataFrame_Iterator(
        std::shared_ptr<MDF_DataSource> dataSource,
        mdf::CANMappingTable mapping,
        uint64_t startTime) : dataSource(dataSource), mapping(mapping), startTime(startTime) {
            index = 0;
            getNextRecord();
    }

    bool CAN_DataFrame_Iterator::operator==(const mdf::CAN_DataFrame_Iterator &other) const {
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

    bool CAN_DataFrame_Iterator::operator!=(const mdf::CAN_DataFrame_Iterator &other) const {
        return !(*this == other);
    }

    std::shared_ptr<CAN_DataFrame_t> CAN_DataFrame_Iterator::operator*() const {
        return value;
    }

    CAN_DataFrame_Iterator& CAN_DataFrame_Iterator::operator++() {
        // Get the next item.
        getNextRecord();

        return *this;
    }

    CAN_DataFrame_Iterator CAN_DataFrame_Iterator::operator++(int) {
        // Create a copy of this object.
        CAN_DataFrame_Iterator returnValue = *this;

        // Increment and return copy.
        ++*this;
        return returnValue;
    }

    void CAN_DataFrame_Iterator::getNextRecord() {
        // Get the next item.
        auto res = dataSource->getRecord(index);

        // If it is a NULL pointer, there is no more data.
        if(res.first) {
            GenericDataRecord const& record = *res.first.get();

            value = std::make_shared<CAN_DataFrame_t>(Convert_Record_CAN_DataFrame(record, mapping));

            // Correct the time.
            value->TimeStamp += startTime;

            index = res.second;
        } else {
            value.reset();
        }
    }

}
