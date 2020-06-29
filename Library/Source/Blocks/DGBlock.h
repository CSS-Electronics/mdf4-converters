#ifndef MDFSIMPLECONVERTERS_DGBLOCK_H
#define MDFSIMPLECONVERTERS_DGBLOCK_H

#include "MdfBlock.h"

#include "CGBlock.h"
#include "DTBlock.h"

namespace mdf {

    struct DGBlock : MdfBlock {
        DGBlock();

        [[nodiscard]] std::shared_ptr<DGBlock> getNextDGBlock() const;
        [[nodiscard]] std::shared_ptr<CGBlock> getFirstCGBlock() const;
        [[nodiscard]] std::shared_ptr<MdfBlock> getDataBlock() const;

        void setNextDGBlock(std::shared_ptr<DGBlock> block);
        void setFirstCGBlock(std::shared_ptr<CGBlock> block);
        void setDataBlock(std::shared_ptr<MdfBlock> block);

        void setRecordSize(uint8_t recordSize);
        uint8_t getRecordSize() const;
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(uint8_t * dataPtr) override;

        uint8_t recordIDSize;
    };

}

#endif //MDFSIMPLECONVERTERS_DGBLOCK_H
