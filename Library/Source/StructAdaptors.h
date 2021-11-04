#ifndef MDFSIMPLECONVERTERS_STRUCTADAPTORS_H
#define MDFSIMPLECONVERTERS_STRUCTADAPTORS_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include "CAN_DataFrame.h"
#include "CAN_ErrorFrame.h"
#include "CAN_RemoteFrame.h"

#include "LIN_ChecksumError.h"
#include "LIN_Frame.h"
#include "LIN_ReceiveError.h"
#include "LIN_SyncError.h"
#include "LIN_TransmissionError.h"

BOOST_FUSION_ADAPT_STRUCT(
    mdf::CAN_DataFrame,
    TimeStamp,
    BusChannel,
    ID,
    IDE,
    DLC,
    DataLength,
    Dir,
    EDL,
    BRS,
    ESI,
    DataBytes
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::CAN_ErrorFrame,
    TimeStamp,
    BusChannel,
    ErrorType
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::CAN_RemoteFrame,
    TimeStamp,
    BusChannel,
    ID,
    IDE,
    DLC,
    DataLength,
    Dir
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::LIN_ChecksumError,
    TimeStamp,
    BusChannel,
    ID,
    Dir,
    ReceivedDataByteCount,
    DataLength,
    Checksum,
    DataBytes
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::LIN_Frame,
    TimeStamp,
    BusChannel,
    ID,
    Dir,
    ReceivedDataByteCount,
    DataLength,
    DataBytes
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::LIN_ReceiveError,
    TimeStamp,
    BusChannel,
    ID
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::LIN_SyncError,
    TimeStamp,
    BusChannel,
    Baudrate
);

BOOST_FUSION_ADAPT_STRUCT(
    mdf::LIN_TransmissionError,
    TimeStamp,
    BusChannel,
    ID
);

#endif //MDFSIMPLECONVERTERS_STRUCTADAPTORS_H
