#include "RecordIterator.h"

#include "CANRecord.h"
#include "LINRecord.h"

namespace mdf {

    template<typename RecordType>
    RecordIterator<RecordType>::RecordIterator() = default;

    template<typename RecordType>
    RecordIterator<RecordType>& RecordIterator<RecordType>::operator=(RecordIterator<RecordType> &&other) noexcept {
        iterator = std::move(other.iterator);

        return *this;
    }

    template<typename RecordType>
    RecordIterator<RecordType>::RecordIterator(std::unique_ptr<IIterator<RecordType>> iterator) : iterator(
        std::move(iterator)) {
        //
    }

    template<typename RecordType>
    RecordIterator<RecordType>::RecordIterator(RecordIterator<RecordType> const &other) {
        iterator = other.iterator->clone();
    }

    template<typename RecordType>
    RecordIterator<RecordType> RecordIterator<RecordType>::begin() const {
        RecordIterator<RecordType> iter(iterator->begin());

        return iter;
    }

    template<typename RecordType>
    RecordIterator<RecordType> RecordIterator<RecordType>::cbegin() const {
        return begin();
    }

    template<typename RecordType>
    RecordIterator<RecordType> RecordIterator<RecordType>::end() const {
        RecordIterator<RecordType> iter(iterator->end());

        return iter;
    }

    template<typename RecordType>
    RecordIterator<RecordType> RecordIterator<RecordType>::cend() const {
        return end();
    }

    template<typename RecordType>
    RecordType const &RecordIterator<RecordType>::dereference() const {
        return iterator->dereference();
    }

    template<typename RecordType>
    void RecordIterator<RecordType>::increment() {
        iterator->increment();
    }

    template<typename RecordType>
    bool RecordIterator<RecordType>::equal(const mdf::RecordIterator<RecordType> &other) const {
        IIterator<RecordType> const *lhs = iterator.get();
        IIterator<RecordType> const *rhs = other.iterator.get();

        bool result = (*lhs == *rhs);

        return result;
    }

    // Initialize for all supported record types.
    template
    struct RecordIterator<CANRecord const>;

    template
    struct RecordIterator<LINRecord const>;

}
