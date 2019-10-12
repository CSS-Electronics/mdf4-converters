#ifndef MDFSORTER_GENERICDATAREADER_H
#define MDFSORTER_GENERICDATAREADER_H

#include <cstdint>
#include <memory>
#include <tuple>

#include "MDF_DataSource.h"

namespace mdf {

    /**
     * MappingInformation
     */
    typedef std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, MDF_DataSource&> MappingInformation;

}

#endif //MDFSORTER_GENERICDATAREADER_H
