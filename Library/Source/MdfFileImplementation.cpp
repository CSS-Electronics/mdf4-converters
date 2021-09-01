#include <memory>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <list>

#include "BlockStorage.h"
#include "ExtractMetaData.h"
#include "MdfFileImplementation.h"
#include "FileInfo/UnfinalizedFileInfo.h"
#include "Utility.h"
#include "RecordIterator.h"
#include "Iterators/EmptyIterator.h"

#include "Blocks/DGBlock.h"
#include "Blocks/DTBlock.h"
#include "Blocks/HDBlock.h"
#include "Blocks/SDBlock.h"
#include "Blocks/DTBlockMultipleRecordIDs.h"
#include "Blocks/DTBlockSingleDiscontinuous.h"
#include "Blocks/DTBlockRaw.h"
#include "Blocks/SIBlock.h"
#include "Blocks/TXBlock.h"
#include "Iterators/GenericIterator.h"
#include "Blocks/DTBlockSingleContinuous.h"
#include "Blocks/SDBlockDiscontinuous.h"

#include "Records/RecordTypes.h"
#include "FileInfo/FinalizedFileInfo.h"

#include "Streams/AESGCMStream.h"
#include "Streams/HeatshrinkStream.h"

namespace mdf {

    MdfFileImplementation::MdfFileImplementation() = default;

    MdfFileImplementation::~MdfFileImplementation() = default;

    bool MdfFileImplementation::finalize() {
        bool result = true;

        // Ensure the file is not already finalized.
        FinalizationFlags flags = idBlock->getFinalizationFlags();

        if (flags == FinalizationFlags_None) {
            return true;
        }

        // Iterate over the finalization flags in the right order.
        if (flags & FinalizationFlags_UpdateLengthInLastDT) {
            result &= finalize_setLengthOfLastDTBlock();
            flags &= (~FinalizationFlags_UpdateLengthInLastDT);
        }

        if (flags & FinalizationFlags_UpdateCycleCounterInCGCA) {
            result &= finalize_updateCycleCountersInCGCABlocks();
            flags &= (~FinalizationFlags_UpdateCycleCounterInCGCA);
        }

        if (flags & FinalizationFlags_UpdateByteCountInVLSDCG) {
            result &= finalize_updateByteCountersInVLSDCGBlocks();
            flags &= (~FinalizationFlags_UpdateByteCountInVLSDCG);
        }

        idBlock->setFinalizationFlags(flags);

        return result;
    }

    bool MdfFileImplementation::finalize_setLengthOfLastDTBlock() {
        // Find the last DT block in the storage.
        std::shared_ptr<MdfBlock> highestDataBlock;
        uint64_t highestDataBlockLocation = 0;

        for (auto &it: blockStorage->getBlockMap().left) {
            if (it.second->getHeader().blockType == MdfBlockType_DT) {
                if (it.first > highestDataBlockLocation) {
                    highestDataBlockLocation = it.first;
                    highestDataBlock = it.second;
                }
            }
        }

        if (highestDataBlock) {
            // Find the next key.
            auto iter = blockStorage->getBlockMap().left.find(highestDataBlockLocation);

            iter++;

            if (iter != std::end(blockStorage->getBlockMap().left)) {
                // TODO: Bound by another block. Use the address of this block to calculate the size. Simply throw for now.
                throw std::runtime_error(
                        "Data block is not bounded by end of file but by another block. Not supported in library.");
            } else {
                MdfHeader dtHeader = highestDataBlock->getHeader();

                // Find end of stream.
                std::streampos const end = stream->pubseekoff(0, std::ios_base::end);

                dtHeader.blockSize = static_cast<uint64_t>(end) - highestDataBlockLocation;
                highestDataBlock->setHeader(dtHeader);
            }
        }

        return true;
    }

    bool MdfFileImplementation::softFinalize_setLengthOfLastDTBlock() {
        // Find the last DT block in the storage.
        std::shared_ptr<MdfBlock> highestDataBlock;
        uint64_t highestDataBlockLocation = 0;

        for (auto &it: blockStorage->getBlockMap().left) {
            if (it.second->getHeader().blockType == MdfBlockType_DT) {
                if (it.first > highestDataBlockLocation) {
                    highestDataBlockLocation = it.first;
                    highestDataBlock = it.second;
                }
            }
        }

        if (highestDataBlock) {
            // Find the smallest possible record which could have been stored here.
            std::shared_ptr<DGBlock> dgBlock = getHDBlock()->getFirstDGBlock();
            int64_t minimumSize = 0;

            while( dgBlock ) {
                if( dgBlock->getDataBlock() == highestDataBlock ) {
                    std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

                    while( cgBlock ) {
                        if( cgBlock->getRecordSize() > minimumSize ) {
                            minimumSize = cgBlock->getRecordSize();
                        }
                        cgBlock = cgBlock->getNextCGBlock();
                    }
                }

                dgBlock = dgBlock->getNextDGBlock();
            }

            // Find the next key.
            auto iter = blockStorage->getBlockMap().left.find(highestDataBlockLocation);

            iter++;

            if (iter != std::end(blockStorage->getBlockMap().left)) {
                // TODO: Bound by another block. Use the address of this block to calculate the size. Simply throw for now.
                throw std::runtime_error(
                        "Data block is not bounded by end of file but by another block. Not supported in library.");
            } else {
                MdfHeader dtHeader = highestDataBlock->getHeader();

                std::streampos const end = stream->pubseekoff(highestDataBlockLocation + minimumSize, std::ios_base::beg);

                dtHeader.blockSize = static_cast<uint64_t>(end) - highestDataBlockLocation;
                highestDataBlock->setHeader(dtHeader);
            }
        }

        return true;
    }

    bool MdfFileImplementation::finalize_updateCycleCountersInCGCABlocks() {
        // Index the records, to get the correct number for each channel group. Loop over each DG block.
        for (auto &it: blockStorage->getBlockMap().left) {
            if (it.second->getHeader().blockType == MdfBlockType_DG) {
                std::shared_ptr<DGBlock> dgBlock = std::dynamic_pointer_cast<DGBlock>(it.second);

                // If multiple CG blocks are found, multiple IDs need to be present.
                std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

                if ((cgBlock) && (cgBlock->getNextCGBlock())) {
                    // Find the data block. Must be multiple record edition.
                    auto dtBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(dgBlock->getDataBlock());

                    if (!dtBlock) {
                        break;
                    }

                    dtBlock->index();
                    auto const counts = dtBlock->getRecordCycleCounts();

                    // Use the record IDs to update cycle counters.
                    while (cgBlock) {
                        // Update counts.
                        uint64_t count = counts.at(cgBlock->getRecordID());
                        cgBlock->setCycleCount(count);

                        // Get handle to the next CG block.
                        cgBlock = cgBlock->getNextCGBlock();
                    }
                } else if ((cgBlock) && (!cgBlock->getNextCGBlock())) {
                    // Find the data block. Must be single record edition.
                    auto dtBlock = std::dynamic_pointer_cast<DTBlockSingleDiscontinuous>(dgBlock->getDataBlock());

                    if (!dtBlock) {
                        break;
                    }

                    // Update the cycle counter.
                }
            }
        }

        return true;
    }

    bool MdfFileImplementation::finalize_updateByteCountersInVLSDCGBlocks() {
        // Loop over DG blocks to locate all VLSD CG blocks.
        for (auto &it: blockStorage->getBlockMap().left) {
            if (it.second->getHeader().blockType == MdfBlockType_DG) {
                std::shared_ptr<DGBlock> dgBlock = std::dynamic_pointer_cast<DGBlock>(it.second);

                // Loop over CG blocks.
                std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

                while (cgBlock) {
                    if (cgBlock->isVLSD()) {
                        // CGVLSD blocks must share a DG block, so this is unsorted data and thus multiple records per
                        // ID.
                        auto dtBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(dgBlock->getDataBlock());

                        uint64_t dataBytes = dtBlock->getTotalSize(cgBlock->getRecordID());
                        cgBlock->setVLSDDataBytes(dataBytes);
                    }

                    // Get handle to the next CG block.
                    cgBlock = cgBlock->getNextCGBlock();
                }
            }
        }

        return true;
    }

    FileInfo MdfFileImplementation::getFileInfo() {
        // If the file is not finalized and sorted yet, do it now.
        finalize();
        sort();

        // Update file information with fields message counts.
        std::shared_ptr<DGBlock> dgBlockCAN = findBUSBlock(RecordType::CAN_DataFrame);

        if(dgBlockCAN) {
            std::shared_ptr<CGBlock> cgBlock = dgBlockCAN->getFirstCGBlock();

            fileInfo.CANMessages = cgBlock->getCycleCount();
        }

        std::shared_ptr<DGBlock> dgBlockLIN = findBUSBlock(RecordType::LIN_Frame);

        if(dgBlockLIN) {
            std::shared_ptr<CGBlock> cgBlock = dgBlockLIN->getFirstCGBlock();

            fileInfo.LINMessages = cgBlock->getCycleCount();
        }

        return fileInfo;
    }

    MetadataMap MdfFileImplementation::getMetadata() const {
        return metadata;
    }

    std::chrono::nanoseconds MdfFileImplementation::getFirstMeasurement() {
        std::chrono::nanoseconds result;

        FinalizationFlags flags = idBlock->getFinalizationFlags();

        if(flags == FinalizationFlags_None) {
            // Finalized file.
            FinalizedFileInfo info(getHDBlock(), stream);

            result = info.firstMeasurement();
        } else {
            // Unfinalized file.
            if (flags & FinalizationFlags_UpdateLengthInLastDT) {
                softFinalize_setLengthOfLastDTBlock();
            }

            idBlock->setFinalizationFlags(flags);

            UnfinalizedFileInfo info(getHDBlock(), stream);

            result = info.firstMeasurement();
        }

        return result;
    }

    bool MdfFileImplementation::loadFileInfo() {
        bool result = true;

        do {
            // Reset info.
            metadata.clear();
            fileInfo.clear();

            // Load data from the HD block.
            {
                std::shared_ptr<HDBlock> hdBlock = getHDBlock();

                // Start time of this measurement.
                uint64_t startTimeNs = hdBlock->getStartTimeNs();
                fileInfo.Time = std::chrono::nanoseconds(startTimeNs);

                // Extract time zone offset.
                fileInfo.TimezoneOffsetMinutes = hdBlock->getTzOffsetMin();

                // Get xml data from MDBlock.
                std::shared_ptr<MDBlock> mdBlock = hdBlock->getComment();

                if (mdBlock) {
                    // Extract common metadata.
                    std::string rawXML(mdBlock->getMetaData());
                    result &= extractMetadataHD(rawXML, metadata);
                }
            }

            // Loop over all DG blocks.
            std::shared_ptr<DGBlock> dgBlock = getHDBlock()->getFirstDGBlock();

            while(dgBlock) {
                // Loop over all CG blocks.
                std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

                while(cgBlock) {
                    // Extract XML from the source information block.
                    std::shared_ptr<SIBlock> siBlock = cgBlock->getSIBlock();

                    if (siBlock) {
                        std::shared_ptr<MDBlock> mdBlock = siBlock->getComment();

                        if (mdBlock) {
                            // Extract common metadata.
                            std::string rawXML(mdBlock->getMetaData());
                            result &= extractMetadataSI(rawXML, metadata);
                        }
                    }

                    cgBlock = cgBlock->getNextCGBlock();
                }

                dgBlock = dgBlock->getNextDGBlock();
            }
        } while (false);

        return result;
    }

    bool MdfFileImplementation::load(std::unique_ptr<std::streambuf> stream_) {
        bool result = false;
        stream = std::move(stream_);

        if(!stream) {
            return result;
        }

        do {
            blockStorage = std::make_unique<BlockStorage>(stream);

            // Load the ID block.
            idBlock = std::make_unique<IDBlock>();
            result = idBlock->load(stream);

            if (!result) {
                break;
            }

            // Load the HD block and all linked blocks.
            blockStorage->getBlockAt(64);
            result = loadFileInfo();
            if (!result) {
                break;
            }
        } while(false);

        return result;
    }

    bool MdfFileImplementation::sort() {
        bool result = false;

        do {
            // Convert VLSD CG blocks to SD blocks.
            result = sort_VLSDCGtoSD();

            if (!result) {
                break;
            }

            // Ensure each DG block is sorted.
            result = sort_CGtoDG();

            if (!result) {
                break;
            }
        } while (false);

        return result;
    }

    bool MdfFileImplementation::sort_VLSDCGtoSD() {
        std::shared_ptr<HDBlock> hdBlock = std::dynamic_pointer_cast<HDBlock>(blockStorage->getBlockAt(64));
        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        // TODO: This construction is a bit convoluted, think of possible simplifications.
        while (dgBlock) {
            std::list<std::shared_ptr<CGBlock>> cgBlocks;
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

            while (cgBlock) {
                cgBlocks.push_back(cgBlock);
                cgBlock = cgBlock->getNextCGBlock();
            }

            // For each VLSD CG block, move it to a SD block and unlink from the chain.
            for (auto const &block: cgBlocks) {
                // Is the next block a VLSD CG block.
                std::shared_ptr<CGBlock> nextCGBlock = block->getNextCGBlock();
                if (nextCGBlock && nextCGBlock->isVLSD()) {
                    // Find all referencing CN blocks.
                    std::vector<std::shared_ptr<CNBlock>> cnBlocks = getAllCNBlocks(block);
                    std::shared_ptr<CNBlock> referencingCNBlock;

                    for (auto &cnBlock: cnBlocks) {
                        if (cnBlock->getDataBlock() == nextCGBlock) {
                            referencingCNBlock = cnBlock;
                            break;
                        }
                    }

                    if (!referencingCNBlock) {
                        continue;
                    }

                    // Extract the records and size information.
                    std::shared_ptr<DTBlockMultipleRecordIDs> dataBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(
                            dgBlock->getDataBlock());
                    uint64_t totalSize = dataBlock->getTotalSize(nextCGBlock->getRecordID());
                    auto records = dataBlock->getRecordIndicesAbsolute(nextCGBlock->getRecordID());

                    totalSize += 4 * nextCGBlock->getCycleCount();

                    // Create a SD block to store the data in.
                    std::shared_ptr<SDBlockDiscontinuous> sdBlock = std::make_shared<SDBlockDiscontinuous>(records, stream);
                    referencingCNBlock->setDataBlock(sdBlock);

                    // Create a DT block without the SD data in it.
                    std::vector<uint64_t> recordsIDs = dataBlock->getRegisteredRecordIDs();
                    recordsIDs.erase(
                            std::remove(std::begin(recordsIDs), std::end(recordsIDs), nextCGBlock->getRecordID()),
                            std::end(recordsIDs));

                    // If only a single ID is left, create a single ID DT block.
                    std::shared_ptr<DTBlock> replacementDTBlock;

                    if (recordsIDs.size() > 1) {
                        replacementDTBlock = std::make_shared<DTBlockMultipleRecordIDs>(*dataBlock, recordsIDs);
                    } else if (recordsIDs.size() == 1) {
                        replacementDTBlock = std::make_shared<DTBlockSingleDiscontinuous>(*dataBlock, recordsIDs[0]);
                    }

                    dgBlock->setDataBlock(replacementDTBlock);

                    // Unlink the CG VLSD block.
                    nextCGBlock = nextCGBlock->getNextCGBlock();
                    block->setNextCGBlock(nextCGBlock);
                }
            }

            // Get the next DG block.
            dgBlock = dgBlock->getNextDGBlock();
        }

        return true;
    }

    bool MdfFileImplementation::sort_CGtoDG() {
        // Move each CG to its own DG group.
        std::shared_ptr<HDBlock> hdBlock = std::dynamic_pointer_cast<HDBlock>(blockStorage->getBlockAt(64));
        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        while (dgBlock) {
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();
            std::shared_ptr<CGBlock> followingCGBlock = cgBlock->getNextCGBlock();

            if (followingCGBlock) {
                // Create a new DG block.
                std::shared_ptr<DGBlock> newDGBlock = std::make_shared<DGBlock>();
                newDGBlock->setFirstCGBlock(followingCGBlock);
                std::shared_ptr<CGBlock> nextCGBlock;
                cgBlock->setNextCGBlock(nextCGBlock);

                // Insert the new DG block between the current DG block and the next.
                newDGBlock->setNextDGBlock(dgBlock->getNextDGBlock());
                dgBlock->setNextDGBlock(newDGBlock);

                // Replace the DT block with a block containing only the relevant ID.
                auto currentDataBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(dgBlock->getDataBlock());
                std::vector<uint64_t> recordsIDs = currentDataBlock->getRegisteredRecordIDs();

                recordsIDs.erase(std::remove(std::begin(recordsIDs), std::end(recordsIDs), cgBlock->getRecordID()),
                                 std::end(recordsIDs));

                auto replacementDTBlockForOldDGBlock = std::make_shared<DTBlockSingleDiscontinuous>(*currentDataBlock,
                                                                                                    cgBlock->getRecordID());

                // Replace the old DT block with one without the removed ID.
                std::shared_ptr<DTBlock> replacementDTBlockForNewDGBlock;
                if (recordsIDs.size() > 1) {
                    replacementDTBlockForNewDGBlock = std::make_shared<DTBlockMultipleRecordIDs>(*currentDataBlock,
                                                                                                 recordsIDs);
                } else if (recordsIDs.size() == 1) {
                    replacementDTBlockForNewDGBlock = std::make_shared<DTBlockSingleDiscontinuous>(*currentDataBlock,
                                                                                                   recordsIDs[0]);
                }

                dgBlock->setDataBlock(replacementDTBlockForOldDGBlock);
                newDGBlock->setDataBlock(replacementDTBlockForNewDGBlock);

                // Clear the record size.
                dgBlock->setRecordSize(0);
            }

            // Get the next DG block.
            dgBlock = dgBlock->getNextDGBlock();
        }

        return true;
    }

    bool MdfFileImplementation::save(std::string fileName) {
        bool result = false;

        // Determine the total file size required.
        uint64_t fileSize = 64;

        for (auto const &iter: blockStorage->getPackedBlockMap()) {
            // Extract current block size.
            uint64_t currentBlockSize = iter.second->getHeader().blockSize;

            // Take 64 bit alignment into account.
            if (currentBlockSize % sizeof(uint64_t) != 0) {
                currentBlockSize += sizeof(uint64_t) - currentBlockSize % sizeof(uint64_t);
            }

            fileSize += currentBlockSize;
        }

        std::filesystem::path p = std::filesystem::current_path() / fileName;
        auto handle = std::fstream(p, std::fstream::binary | std::fstream::out | std::fstream::trunc);
        auto* buf = handle.rdbuf();

        // Save the ID block.
        idBlock->save(buf);

        // Get a list of all linked blocks in a packed configuration.
        auto blocks = blockStorage->getPackedBlockMap();

        // Update the file locations.
        for (auto &block: blocks) {
            block.second->setFileLocation(block.first);
        }

        // Save all the blocks.
        for (auto &block: blocks) {
            buf->pubseekoff(block.first, std::ios_base::beg);
            block.second->save(buf);
        }

        return result;
    }

    std::shared_ptr<HDBlock> MdfFileImplementation::getHDBlock() const {
        std::shared_ptr<HDBlock> hdBlock = std::dynamic_pointer_cast<HDBlock>(blockStorage->getBlockAt(64));

        return hdBlock;
    }

    std::shared_ptr<DGBlock> MdfFileImplementation::findBUSBlock(RecordType recordType) const {
        // Find the correct DG block.
        std::shared_ptr<HDBlock> hdBlock = getHDBlock();
        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        std::shared_ptr<DGBlock> targetDGBlock;

        while (dgBlock) {
            // Check the the DG block is sorted.
            do {
                std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();
                if (cgBlock->getNextCGBlock()) {
                    break;
                }

                // Ensure the the data is bus logging and of the correct bus type.
                if ((cgBlock->getFlags() & CGBlockFlags::BUSEvents) != CGBlockFlags::BUSEvents) {
                    break;
                }

                // Check the name.
                if(cgBlock->getTXBlock()->getText() != getRecordName(recordType))
                {
                    break;
                }

                targetDGBlock = dgBlock;
            } while (false);

            if (targetDGBlock) {
                break;
            }

            // Continue with the next linked DG block.
            dgBlock = dgBlock->getNextDGBlock();
        }

        return targetDGBlock;
    }

    RecordIterator<CAN_DataFrame const> MdfFileImplementation::getCANIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the CAN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::CAN_DataFrame);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<CAN_DataFrame const>> iter = std::make_unique<GenericIterator<CAN_DataFrame>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<CAN_DataFrame const>(std::move(iter));
        }

        throw std::runtime_error("Not finalized?");
    }

    RecordIterator<CAN_ErrorFrame const> MdfFileImplementation::getCANErrorFrameIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the CAN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::CAN_ErrorFrame);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator < CAN_ErrorFrame const>> iter = std::make_unique<GenericIterator<CAN_ErrorFrame>>(
                dgBlock, stream,
                fileInfo.Time);

            return RecordIterator < CAN_ErrorFrame const>(std::move(iter));
        } else {
            return RecordIterator<CAN_ErrorFrame const>(std::make_unique<EmptyIterator<CAN_ErrorFrame>>());
        }
    }

    RecordIterator<CAN_RemoteFrame const> MdfFileImplementation::getCANRemoteFrameIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the CAN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::CAN_RemoteFrame);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<CAN_RemoteFrame const>> iter = std::make_unique<GenericIterator<CAN_RemoteFrame>>(dgBlock, stream,
                                                                                                                      fileInfo.Time);

            return RecordIterator<CAN_RemoteFrame const>(std::move(iter));
        } else {
            return RecordIterator<CAN_RemoteFrame const>(std::make_unique<EmptyIterator<CAN_RemoteFrame>>());
        }
    }

    RecordIterator<LIN_ChecksumError const> MdfFileImplementation::getLINChecksumErrorIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::LIN_ChecksumError);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LIN_ChecksumError const>> iter = std::make_unique<GenericIterator<LIN_ChecksumError>>(dgBlock, stream,
                                                                                                                    fileInfo.Time);

            return RecordIterator<LIN_ChecksumError const>(std::move(iter));
        } else {
            return RecordIterator<LIN_ChecksumError const>(std::make_unique<EmptyIterator<LIN_ChecksumError>>());
        }
    }

    RecordIterator<LIN_Frame const> MdfFileImplementation::getLINIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::LIN_Frame);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LIN_Frame const>> iter = std::make_unique<GenericIterator<LIN_Frame>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<LIN_Frame const>(std::move(iter));
        } else {
            return RecordIterator<LIN_Frame const>(std::make_unique<EmptyIterator<LIN_Frame>>());
        }
    }

    RecordIterator<LIN_ReceiveError const> MdfFileImplementation::getLINReceiveErrorIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::LIN_ReceiveError);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LIN_ReceiveError const>> iter = std::make_unique<GenericIterator<LIN_ReceiveError>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<LIN_ReceiveError const>(std::move(iter));
        } else {
            return RecordIterator<LIN_ReceiveError const>(std::make_unique<EmptyIterator<LIN_ReceiveError>>());
        }
    }

    RecordIterator<LIN_SyncError const> MdfFileImplementation::getLINSyncErrorIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::LIN_SyncError);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LIN_SyncError const>> iter = std::make_unique<GenericIterator<LIN_SyncError>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<LIN_SyncError const>(std::move(iter));
        } else {
            return RecordIterator<LIN_SyncError const>(std::make_unique<EmptyIterator<LIN_SyncError>>());
        }
    }

    RecordIterator<LIN_TransmissionError const> MdfFileImplementation::getLINTransmissionErrorIterator() {
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(RecordType::LIN_TransmissionError);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LIN_TransmissionError const>> iter = std::make_unique<GenericIterator<LIN_TransmissionError>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<LIN_TransmissionError const>(std::move(iter));
        } else {
            return RecordIterator<LIN_TransmissionError const>(std::make_unique<EmptyIterator<LIN_TransmissionError>>());
        }
    }

}
