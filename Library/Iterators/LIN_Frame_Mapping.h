#ifndef MDFSORTER_LIN_FRAME_MAPPING_H
#define MDFSORTER_LIN_FRAME_MAPPING_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include <boost/assign/list_of.hpp>
#include <bits/shared_ptr.h>
#include "GenericDataRecord.h"
#include "LIN_Frame.h"
#include "MDF_Block.h"

namespace mdf {

    /**
     * List of supported fields for the CAN_DataFrame.
     */
    enum LIN_Frame_Fields {
        LIN_Timestamp,
        LIN_BusChannel,
        LIN_ID,
        LIN_DataLength,
        LIN_ReceivedDataByteCount,
        LIN_Dir,
        LIN_DataBytes,
    };

    // Byte, bit, length.
    typedef std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, std::shared_ptr<SD_Block>> MappingInformation;
    typedef std::map<LIN_Frame_Fields, MappingInformation> LINMappingTable;

    std::map<std::string, LIN_Frame_Fields> const LIN_Frame_FieldMapping = boost::assign::map_list_of
        ("Timestamp", LIN_Timestamp)
        ("LIN_Frame.BusChannel", LIN_BusChannel)
        ("LIN_Frame.ID", LIN_ID)
        ("LIN_Frame.DataLength", LIN_DataLength)
        ("LIN_Frame.ReceivedDataByteCount", LIN_ReceivedDataByteCount)
        ("LIN_Frame.Dir", LIN_Dir)
        ("LIN_Frame.DataBytes", LIN_DataBytes);

    LIN_Frame_t Convert_Record_LIN_Frame(GenericDataRecord const& data, LINMappingTable const& mapping);
}

#endif //MDFSORTER_LIN_FRAME_MAPPING_H
