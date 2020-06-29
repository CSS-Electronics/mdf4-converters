#ifndef MDFSIMPLECONVERTERS_UTILITY_H
#define MDFSIMPLECONVERTERS_UTILITY_H

#include "Blocks/CGBlock.h"
#include "Blocks/CNBlock.h"

#include <memory>
#include <vector>

namespace mdf {
    std::vector<std::shared_ptr<CNBlock>> getAllCNBlocks(std::shared_ptr<CGBlock> parent);

    std::vector<std::shared_ptr<CNBlock>> getAllCNBlocks(std::shared_ptr<CNBlock> parent);

    bool iequals(const std::string &a, const std::string &b);
}


#endif //MDFSIMPLECONVERTERS_UTILITY_H
