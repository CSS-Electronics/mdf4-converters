#ifndef MDFSORTER_SD_BLOCK_H
#define MDFSORTER_SD_BLOCK_H

#include "MDF_Block.h"
#include "MDF_DataSource.h"

namespace mdf {

    struct SD_Block : public MDF_Block, public MDF_DataSource {
        // Constructors.
        SD_Block();
        SD_Block(std::shared_ptr<DT_Block> dtBlock, std::shared_ptr<CG_Block> cgBlock);

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        // Setters.

        // Others.
        uint64_t recordID;
        std::shared_ptr<DT_Block> dataSource;

        std::pair<std::shared_ptr<GenericDataRecord>, std::size_t> getRecord(std::size_t index) override;

    private:
        uint64_t lastMappedIndex;
        uint64_t lastIndex;
    };

}

#endif //MDFSORTER_SD_BLOCK_H
