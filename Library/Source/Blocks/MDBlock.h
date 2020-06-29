#ifndef MDFSIMPLECONVERTERS_MDBLOCK_H
#define MDFSIMPLECONVERTERS_MDBLOCK_H

#include <string_view>

#include "MdfBlock.h"

namespace mdf {

    struct MDBlock : MdfBlock {
      std::string_view getMetaData() const;
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(std::streambuf *stream) override;
    private:
        std::string metaData;
    };

}

#endif //MDFSIMPLECONVERTERS_MDBLOCK_H
