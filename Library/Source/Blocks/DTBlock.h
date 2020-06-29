#ifndef MDFSIMPLECONVERTERS_DTBLOCK_H
#define MDFSIMPLECONVERTERS_DTBLOCK_H

#include "MdfBlock.h"
#include "IDataBlock.h"

namespace mdf {

    /**
     * The DT block in general contains constant length record data. The data can be represented as sorted or
     * unsorted. In the sorted case, all records are from the same source, and thus have the same length. In the
     * unsorted case, multiple sources store data in the same block, so the length is no longer guaranteed to be the
     * same.
     * When loading the DT block, it is not possible to determine if it is either-or. This can only be deduced from
     * the remaining structure, in terms of blocks linking to the DT block.
     */
    struct DTBlock : MdfBlock, IDataBlock {
        /**
         * Default constructor.
         */
        DTBlock();

    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;

        std::shared_ptr<std::streambuf> stream;
        uint64_t rawFileLocation;
    };

}

#endif //MDFSIMPLECONVERTERS_DTBLOCK_H
