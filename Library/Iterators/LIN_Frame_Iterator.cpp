#include "LIN_Frame_Iterator.h"

namespace mdf {

    LIN_Frame_Iterator::LIN_Frame_Iterator() {
        //
    }

    LIN_Frame_Iterator::LIN_Frame_Iterator(
        std::shared_ptr<MDF_DataSource> dataSource,
        mdf::LINMappingTable mapping,
        uint64_t startTime) : dataSource(dataSource), mapping(mapping), startTime(startTime) {
        index = 0;
        getNextRecord();
    }

    bool LIN_Frame_Iterator::operator==(const mdf::LIN_Frame_Iterator &other) const {
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

    bool LIN_Frame_Iterator::operator!=(const mdf::LIN_Frame_Iterator &other) const {
        return !(*this == other);
    }

    std::shared_ptr<LIN_Frame_t> LIN_Frame_Iterator::operator*() const {
        return value;
    }

    LIN_Frame_Iterator& LIN_Frame_Iterator::operator++() {
        // Get the next item.
        getNextRecord();

        return *this;
    }

    LIN_Frame_Iterator const LIN_Frame_Iterator::operator++(int) {
        // Create a copy of this object.
        LIN_Frame_Iterator returnValue = *this;

        // Increment and return copy.
        ++*this;
        return returnValue;
    }

    void LIN_Frame_Iterator::getNextRecord() {
        // Get the next item.
        auto res = dataSource->getRecord(index);

        // If it is a NULL pointer, there is no more data.
        if(res.first) {
            GenericDataRecord const& record = *res.first.get();

            value = std::make_shared<LIN_Frame_t>(Convert_Record_LIN_Frame(record, mapping));

            // Correct the time.
            value->TimeStamp += startTime;

            index = res.second;
        } else {
            value.reset();
        }
    }

}
