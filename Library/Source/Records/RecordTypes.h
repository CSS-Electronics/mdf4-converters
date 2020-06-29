#ifndef MDFSUPER_RECORDTYPES_H
#define MDFSUPER_RECORDTYPES_H

#include <string_view>

namespace mdf {

    enum struct RecordType {
        CAN_DataFrame,
        CAN_ErrorFrame,
        CAN_RemoteFrame,
        LIN_ChecksumError,
        LIN_Frame,
        LIN_ReceiveError,
        LIN_SyncError,
        LIN_TransmissionError,
    };

    std::string_view getRecordName(RecordType recordType);

}

#endif //MDFSUPER_RECORDTYPES_H
