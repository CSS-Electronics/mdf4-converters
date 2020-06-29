#include "RecordIterator.h"

#include "CAN_DataFrame.h"
#include "CAN_ErrorFrame.h"
#include "CAN_RemoteFrame.h"
#include "LIN_ChecksumError.h"
#include "LIN_Frame.h"
#include "LIN_ReceiveError.h"
#include "LIN_SyncError.h"
#include "LIN_TransmissionError.h"

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
    struct RecordIterator<CAN_DataFrame const>;

    template
    struct RecordIterator<CAN_ErrorFrame const>;

    template
    struct RecordIterator<CAN_RemoteFrame const>;

    template
    struct RecordIterator<LIN_ChecksumError const>;

    template
    struct RecordIterator<LIN_Frame const>;

    template
    struct RecordIterator<LIN_ReceiveError const>;

    template
    struct RecordIterator<LIN_SyncError const>;

    template
    struct RecordIterator<LIN_TransmissionError const>;

}
