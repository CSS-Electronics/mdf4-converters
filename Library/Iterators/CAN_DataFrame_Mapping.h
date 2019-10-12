#ifndef MDFSORTER_CAN_DATAFRAME_MAPPING_H
#define MDFSORTER_CAN_DATAFRAME_MAPPING_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include <boost/assign/list_of.hpp>
#include <bits/shared_ptr.h>
#include "GenericDataRecord.h"
#include "CAN_DataFrame.h"
#include "MDF_Block.h"

namespace mdf {

    /**
     * List of supported fields for the CAN_DataFrame.
     */
    enum CAN_DataFrame_Fields {
        CAN_Timestamp,
        CAN_BusChannel,
        CAN_ID,
        CAN_IDE,
        CAN_DLC,
        CAN_DataLength,
        CAN_Dir,
        CAN_EDL,
        CAN_BRS,
        CAN_DataBytes,
    };

    // Byte, bit, length.
    typedef std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, std::shared_ptr<SD_Block>> MappingInformation;
    typedef std::map<CAN_DataFrame_Fields, MappingInformation> CANMappingTable;

    std::map<std::string, CAN_DataFrame_Fields> const CAN_DataFrame_FieldMapping = boost::assign::map_list_of
        ("Timestamp", CAN_Timestamp)
        ("CAN_DataFrame.BusChannel", CAN_BusChannel)
        ("CAN_DataFrame.ID", CAN_ID)
        ("CAN_DataFrame.IDE", CAN_IDE)
        ("CAN_DataFrame.DLC", CAN_DLC)
        ("CAN_DataFrame.DataLength", CAN_DataLength)
        ("CAN_DataFrame.Dir", CAN_Dir)
        ("CAN_DataFrame.EDL", CAN_EDL)
        ("CAN_DataFrame.BRS", CAN_BRS)
        ("CAN_DataFrame.DataBytes", CAN_DataBytes);

    CAN_DataFrame_t Convert_Record_CAN_DataFrame(GenericDataRecord const& data, CANMappingTable const& mapping);
}

#endif //MDFSORTER_CAN_DATAFRAME_MAPPING_H
