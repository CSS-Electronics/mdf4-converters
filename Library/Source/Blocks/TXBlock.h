#ifndef MDFSIMPLECONVERTERS_TXBLOCK_H
#define MDFSIMPLECONVERTERS_TXBLOCK_H

#include <string_view>

#include "MdfBlock.h"

namespace mdf {

    struct TXBlock : MdfBlock {
    public:
        std::string_view getText() const;
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(std::streambuf *stream) override;
    private:
        std::string text;
    };

}

#endif //MDFSIMPLECONVERTERS_MDBLOCK_H
