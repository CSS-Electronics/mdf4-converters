#ifndef MDFSUPER_FINALIZEDFILEINFO_H
#define MDFSUPER_FINALIZEDFILEINFO_H

#include <chrono>
#include <streambuf>

#include "../Blocks/HDBlock.h"

namespace mdf {

    struct FinalizedFileInfo {
        FinalizedFileInfo(std::shared_ptr<HDBlock> hdBlock, std::shared_ptr<std::streambuf> dataSource);

        std::chrono::nanoseconds firstMeasurement() const;
    private:
        std::shared_ptr<HDBlock> hdBlock;
        std::shared_ptr<std::streambuf> dataSource;
    };


}

#endif //MDFSUPER_FINALIZEDFILEINFO_H
