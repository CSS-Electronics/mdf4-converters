#ifndef MDFSORTER_MDF_MISSINGBLOCKERROR_H
#define MDFSORTER_MDF_MISSINGBLOCKERROR_H

#include "MDF_Exception.h"

#include "MDF_Block.h"

namespace mdf {

    struct MDF_MissingBlockError : public MDF_Exception {
        static MDF_MissingBlockError Create(std::shared_ptr<MDF_Block> block, std::string const& missingMember);

        explicit MDF_MissingBlockError(std::string const& what);
    };

}

#endif //MDFSORTER_MDF_MISSINGBLOCKERROR_H
