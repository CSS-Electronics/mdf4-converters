#include "ID_Block.h"

namespace mdf {

    FinalizationFlags ID_Block::getFinalizationFlags() const {
        return data.finalizationFlags;
    }

    void ID_Block::setFinalizationFlags(mdf::FinalizationFlags &flags) {
        data.finalizationFlags = flags;
    }

    ID_Block::ID_Block() {
        // Set constant data for the header and file location.
        header.type = MDF_Type_ID;
        header.linkCount = 0;
        header.size = sizeof(data);

        fileLocation = 0;

        // TODO: Initialize the data with sane default.
    }

    bool ID_Block::load(std::istream &stream) {
        // Map the memory directly into the data block.
        char* dataPtr = reinterpret_cast<char*>(&data);
        stream.read(dataPtr, sizeof(ID_Data));

        // Did the load succeed?
        return !stream.fail();
    }

    void ID_Block::save(std::ostream &stream) {
        char* dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(ID_Data));
    }

}
