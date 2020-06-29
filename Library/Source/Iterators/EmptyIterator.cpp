#include "EmptyIterator.h"

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
    EmptyIterator<RecordType>::EmptyIterator() {
        //
    }

    template<typename RecordType>
    void EmptyIterator<RecordType>::increment() {
        //
    }

    template<typename RecordType>
    RecordType const &EmptyIterator<RecordType>::dereference() {
        throw;
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> EmptyIterator<RecordType>::clone() const {
        return std::make_unique<EmptyIterator<RecordType>>(*this);
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> EmptyIterator<RecordType>::begin() const {
        auto result = std::make_unique<EmptyIterator<RecordType>>(*this);

        return result;
    }

    template<typename RecordType>
    std::unique_ptr<IIterator<RecordType const>> EmptyIterator<RecordType>::end() const {
        auto result = std::make_unique<EmptyIterator<RecordType>>(*this);

        return result;
    }

    template<typename RecordType>
    bool EmptyIterator<RecordType>::equals(IIterator<RecordType const> const &other) const {
        return true;
    }

    // Initialize for all supported record types.
    template
    struct EmptyIterator<CAN_DataFrame>;

    template
    struct EmptyIterator<CAN_ErrorFrame>;

    template
    struct EmptyIterator<CAN_RemoteFrame>;

    template
    struct EmptyIterator<LIN_ChecksumError>;

    template
    struct EmptyIterator<LIN_Frame>;

    template
    struct EmptyIterator<LIN_ReceiveError>;

    template
    struct EmptyIterator<LIN_SyncError>;

    template
    struct EmptyIterator<LIN_TransmissionError>;

}
