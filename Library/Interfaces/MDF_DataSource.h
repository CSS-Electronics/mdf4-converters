#ifndef MDFSORTER_MDF_DATASOURCE_H
#define MDFSORTER_MDF_DATASOURCE_H

#include <memory>

#include "GenericDataRecord.h"

namespace mdf {

    struct MDF_DataSource {
        virtual std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> getRecord(std::size_t index) = 0;
    };

}

#endif //MDFSORTER_MDF_DATASOURCE_H
