#ifndef MDFSIMPLECONVERTERS_SDBLOCK_H
#define MDFSIMPLECONVERTERS_SDBLOCK_H

#include "MdfBlock.h"
#include "IDataBlock.h"

namespace mdf {

  struct SDBlock : MdfBlock, IDataBlock {
    SDBlock();
  };

}

#endif //MDFSIMPLECONVERTERS_SDBLOCK_H
