#ifndef MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H
#define MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H

#include <memory>

#include "MdfFile.h"
#include "BlockStorage.h"
#include "Blocks/DGBlock.h"
#include "Blocks/HDBlock.h"
#include "Blocks/IDBlock.h"
#include "Records/RecordTypes.h"

namespace mdf {

    struct MdfFileImplementation : MdfFile {
        MdfFileImplementation();

        ~MdfFileImplementation();

        bool finalize() override;

        bool finalize_setLengthOfLastDTBlock();

        bool softFinalize_setLengthOfLastDTBlock();

        bool finalize_updateCycleCountersInCGCABlocks();

        bool finalize_updateByteCountersInVLSDCGBlocks();

        bool sort() override;

        bool sort_VLSDCGtoSD();

        bool sort_CGtoDG();

        MetadataMap getMetadata() const override;

        FileInfo getFileInfo() override;

        bool load(std::unique_ptr<std::streambuf> stream);

        std::chrono::nanoseconds getFirstMeasurement() override;

        bool save(std::string fileName) override;

        RecordIterator<CAN_DataFrame const> getCANIterator() override;

        RecordIterator<CAN_ErrorFrame const> getCANErrorFrameIterator() override;

        RecordIterator<CAN_RemoteFrame const> getCANRemoteFrameIterator() override;

        RecordIterator<LIN_ChecksumError const> getLINChecksumErrorIterator() override;

        RecordIterator<LIN_Frame const> getLINIterator() override;

        RecordIterator<LIN_ReceiveError const> getLINReceiveErrorIterator() override;

        RecordIterator<LIN_SyncError const> getLINSyncErrorIterator() override;

        RecordIterator<LIN_TransmissionError const> getLINTransmissionErrorIterator() override;

    private:
        [[nodiscard]] std::shared_ptr<HDBlock> getHDBlock() const;

        [[nodiscard]] std::shared_ptr<DGBlock> findBUSBlock(RecordType recordType) const;

        bool loadFileInfo();
        FileInfo fileInfo;

        MetadataMap metadata;

        std::unique_ptr<BlockStorage> blockStorage;
        std::unique_ptr<IDBlock> idBlock;
        std::shared_ptr<std::streambuf> stream;
    };

}

#endif //MDFSIMPLECONVERTERS_MDFFILEIMPLEMENTATION_H
