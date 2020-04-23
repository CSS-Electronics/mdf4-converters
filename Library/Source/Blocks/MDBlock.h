#ifndef MDFSIMPLECONVERTERS_MDBLOCK_H
#define MDFSIMPLECONVERTERS_MDBLOCK_H

#include "MdfBlock.h"

namespace mdf {

    struct MDBlock : MdfBlock {
      std::string_view getMetaData() const;
    protected:
        bool load(uint8_t const* dataPtr) override;
        bool saveBlockData(uint8_t * dataPtr) override;
    private:
        std::string metaData;
    };

}

#endif //MDFSIMPLECONVERTERS_MDBLOCK_H
