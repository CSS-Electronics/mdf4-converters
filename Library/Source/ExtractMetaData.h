#ifndef MDFSIMPLECONVERTERS_EXTRACTMETADATA_H
#define MDFSIMPLECONVERTERS_EXTRACTMETADATA_H

#include "Metadata.h"

namespace mdf {

    bool extractMetadata(std::string const& xml, MetadataMap& metadata);

    bool extractMetadataHD(std::string const& xml, MetadataMap& metadata);

    bool extractMetadataSI(std::string const& xml, MetadataMap& metadata);

}

#endif //MDFSIMPLECONVERTERS_EXTRACTMETADATA_H
