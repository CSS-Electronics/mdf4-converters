#include "RecordTypes.h"

namespace mdf {

    std::string_view getRecordName(RecordType recordType) {
        std::string_view result = "";

        switch(recordType) {
            case RecordType::CAN_DataFrame:
            {
                result = "CAN_DataFrame";
                break;
            }
            case RecordType::CAN_ErrorFrame:
            {
                result = "CAN_ErrorFrame";
                break;
            }
            case RecordType::CAN_RemoteFrame:
            {
                result = "CAN_RemoteFrame";
                break;
            }
            case RecordType::LIN_ChecksumError:
            {
                result = "LIN_ChecksumError";
                break;
            }
            case RecordType::LIN_Frame:
            {
                result = "LIN_Frame";
                break;
            }
            case RecordType::LIN_ReceiveError:
            {
                result = "LIN_ReceiveError";
                break;
            }
            case RecordType::LIN_SyncError:
            {
                result = "LIN_SyncError";
                break;
            }
            case RecordType::LIN_TransmissionError:
            {
                result = "LIN_TransmissionError";
                break;
            }
            default:
                break;
        }

        return result;
    }

}
