#ifndef MDFSUPER_FILEINFOUTILITY_H
#define MDFSUPER_FILEINFOUTILITY_H

#include <chrono>
#include <streambuf>

#include "../Blocks/CGBlock.h"

namespace mdf {

    std::shared_ptr<CNBlock> getMasterTimeChannel(std::shared_ptr<CGBlock> cgBlock);

}

#endif //MDFSUPER_FILEINFOUTILITY_H
