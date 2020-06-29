#ifndef MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H
#define MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H

#include <memory>

#include "MdfFile.h"
#include "BlockStorage.h"
#include "Blocks/DGBlock.h"
#include "Blocks/HDBlock.h"
#include "Blocks/IDBlock.h"

namespace mdf {

    struct MdfFileImplementation : MdfFile {
        MdfFileImplementation();

        ~MdfFileImplementation();

        bool finalize() override;

        bool finalize_setLengthOfLastDTBlock();

        bool finalize_updateCycleCountersInCGCABlocks();

        bool finalize_updateByteCountersInVLSDCGBlocks();

        bool sort() override;

        bool sort_VLSDCGtoSD();

        bool sort_CGtoDG();

        MetadataMap getMetadata() const override;

        FileInfo getFileInfo() override;

        bool load(std::shared_ptr<std::streambuf> stream);

        std::chrono::nanoseconds getFirstMeasurement() override;

        bool save(std::string fileName) override;

        RecordIterator<CANRecord const> getCANIterator() override;

        RecordIterator<LINRecord const> getLINIterator() override;

    private:
        [[nodiscard]] std::shared_ptr<HDBlock> getHDBlock() const;

        [[nodiscard]] std::shared_ptr<DGBlock> findBUSBlock(SIBlockBusType busType) const;

        bool loadFileInfo();
        FileInfo fileInfo;

        MetadataMap metadata;

        std::unique_ptr<BlockStorage> blockStorage;
        std::unique_ptr<IDBlock> idBlock;
        std::shared_ptr<std::streambuf> stream;
    };

}

#endif //MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H
