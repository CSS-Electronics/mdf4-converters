#ifndef MDFSUPER_METADATA_H
#define MDFSUPER_METADATA_H

#include <map>
#include <string>

namespace mdf {

    struct MdfMetadataEntry {
        bool readOnly = false;
        std::string name = "";
        std::string description = "";
        std::string unit = "";
        std::string valueRaw = "string";
        std::string valueType = "";
    };

    typedef std::map<std::string, MdfMetadataEntry> MetadataMap;

}

#endif //MDFSUPER_METADATA_H
