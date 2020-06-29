#ifndef MDFSUPER_UNFINALIZEDFILEINFO_H
#define MDFSUPER_UNFINALIZEDFILEINFO_H

#include <chrono>
#include <streambuf>

#include "Blocks/HDBlock.h"

namespace mdf {

    struct UnfinalizedFileInfo {
        UnfinalizedFileInfo(std::shared_ptr<HDBlock> hdBlock, std::shared_ptr<std::streambuf> dataSource);

        std::chrono::nanoseconds firstMeasurement() const;
    private:
        std::shared_ptr<HDBlock> hdBlock;
        std::shared_ptr<std::streambuf> dataSource;
    };


}

#endif //MDFSUPER_UNFINALIZEDFILEINFO_H
