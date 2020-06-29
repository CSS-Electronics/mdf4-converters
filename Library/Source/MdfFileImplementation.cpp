#include <memory>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <list>

#include "BlockStorage.h"
#include "ExtractMetaData.h"
#include "MdfFileImplementation.h"
#include "UnfinalizedFileInfo.h"
#include "Utility.h"
#include "RecordIterator.h"

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

namespace mdf {

    MdfFileImplementation::MdfFileImplementation() {

    }

    MdfFileImplementation::~MdfFileImplementation() {
        //
    }

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
        std::shared_ptr<DGBlock> dgBlockCAN = findBUSBlock(SIBlockBusType::CAN);

        if(dgBlockCAN) {
            std::shared_ptr<CGBlock> cgBlock = dgBlockCAN->getFirstCGBlock();

            fileInfo.CANMessages = cgBlock->getCycleCount();
        }

        std::shared_ptr<DGBlock> dgBlockLIN = findBUSBlock(SIBlockBusType::LIN);

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
        FinalizationFlags flags = idBlock->getFinalizationFlags();

        if (flags & FinalizationFlags_UpdateLengthInLastDT) {
            finalize_setLengthOfLastDTBlock();
            flags &= (~FinalizationFlags_UpdateLengthInLastDT);
        }

        idBlock->setFinalizationFlags(flags);

        UnfinalizedFileInfo info(getHDBlock(), stream);

        return info.firstMeasurement();
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

    bool MdfFileImplementation::load(std::shared_ptr<std::streambuf> stream_) {
        bool result = false;
        stream = stream_;

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
        // Sorting requires each DG block to have only a single CG block. Thus, loop over all DG blocks, and for each
        // CG block after the first, move it to a new DG block.
        // For VLSD CG blocks, instead of moving them to a new DG block, convert them into a SD block, and unlink them
        // from the CG block chain (But ensure the link in the CN block referencing it stays intact).
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

        /*
        // Post sorting, each DT block can only contain a single record ID. If the file was sorted this run, they are
        // replaced with in-memory mapped DT blocks. If it was sorted prior to this run, replace the DT blocks.
        std::shared_ptr<DGBlock> dgBlock = getHDBlock()->getFirstDGBlock();

        while(dgBlock) {
          // Loop over all linked DT blocks and replace them if not already single ID blocks.

          // Check if it is of the right type.
          std::shared_ptr<DTBlock> dtBlock = std::dynamic_pointer_cast<DTBlock>(dgBlock->getDataBlock());

          if(dtBlock) {
            // Ensure it is a single ID block.
            std::shared_ptr<DTBlockRaw> dtBlockRaw = std::dynamic_pointer_cast<DTBlockRaw>(dtBlock);

            if(!dtBlockRaw) {
              // Extract record size.
              std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();
              std::size_t recordSize = cgBlock->getRecordSize();

              // Convert into a single block ID.
              std::shared_ptr<DTBlockSingleContinuous> replacementDTBlock;

              replacementDTBlock = std::make_shared<DTBlockSingleContinuous>(*dtBlockRaw, recordSize);

              // Replace original block.
              dgBlock->setDataBlock(replacementDTBlock);
            }

          }

          // Select next data group.
          dgBlock = dgBlock->getNextDGBlock();
        }*/

        return result;
    }

    bool MdfFileImplementation::sort_VLSDCGtoSD() {
        // For each DG block, find all CG blocks. For each CG block which is a VLSD block, move it to a SD block
        // instead.
        std::shared_ptr<HDBlock> hdBlock = std::dynamic_pointer_cast<HDBlock>(blockStorage->getBlockAt(64));
        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        // TODO: This construction is a bit convoluted, think of possible simplifications.
        while (dgBlock) {
            // Create a double linked list of all CG blocks in this DG block.
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
            // Get the first CG block.
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

            // Any following CG blocks?
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
        /*
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

        // Open a memory mapped file with this size. First, create it.
        std::filesystem::path p = std::filesystem::current_path() / fileName;

        std::ofstream(p).put('\0');
        std::filesystem::resize_file(p, fileSize);

        // Next, load it as a writeable memory map.
        std::error_code error;
        mio::mmap_sink outputFile;
        outputFile.map(fileName, error);

        // Save the ID block.
        idBlock->save(reinterpret_cast<uint8_t *>(outputFile.data()));

        // Get a list of all linked blocks in a packed configuration.
        auto blocks = blockStorage->getPackedBlockMap();

        // Update the file locations.
        for (auto &block: blocks) {
            block.second->setFileLocation(block.first);
        }

        // Save all the blocks.
        for (auto &block: blocks) {
            uint8_t *dataPtr = reinterpret_cast<uint8_t *>(outputFile.data()) + block.first;
            block.second->save(dataPtr);
        }
*/
        return result;
    }

    std::shared_ptr<HDBlock> MdfFileImplementation::getHDBlock() const {
        std::shared_ptr<HDBlock> hdBlock = std::dynamic_pointer_cast<HDBlock>(blockStorage->getBlockAt(64));

        return hdBlock;
    }

    std::shared_ptr<DGBlock> MdfFileImplementation::findBUSBlock(SIBlockBusType busType) const {
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

                // Ensure the data is of the correct bus type.
                std::shared_ptr<SIBlock> siBlock = cgBlock->getSIBlock();

                if (!siBlock) {
                    // No valid source information block, can not determine if this is the correct bus type.
                    break;
                }

                if ((siBlock->getBusType() & busType) != busType) {
                    // Not correct bus type.
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

    RecordIterator<CANRecord const> MdfFileImplementation::getCANIterator() {
        /*
        std::shared_ptr<DGBlock> dgBlock = getHDBlock()->getFirstDGBlock();
        std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

        std::unique_ptr<IIterator<CANRecord const>> iter;
        iter = std::make_unique<CANIterator>(
            dgBlock,
            cgBlock,
            stream,
            fileInfo.Time
            );

        return RecordIterator<CANRecord const>(std::move(iter));







        */
        // Ensure result is finalized and sorted.
        auto flags = idBlock->getFinalizationFlags();

        if(flags != FinalizationFlags_None) {
            bool result = true;

            result &= finalize();
            result &= sort();
            result &= loadFileInfo();
        }

        // Find the CAN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(SIBlockBusType::CAN);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<CANRecord const>> iter = std::make_unique<GenericIterator<CANRecord>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<CANRecord const>(std::move(iter));
        }

        throw std::runtime_error("Not finalized?");
        return RecordIterator<CANRecord const>();
    }

    RecordIterator<LINRecord const> MdfFileImplementation::getLINIterator() {
        // Find the LIN DG block.
        std::shared_ptr<DGBlock> dgBlock = findBUSBlock(SIBlockBusType::LIN);

        if (dgBlock) {
            // Create a new iterator from this.
            std::unique_ptr<IIterator<LINRecord const>> iter = std::make_unique<GenericIterator<LINRecord>>(dgBlock, stream,
                                                                                                            fileInfo.Time);

            return RecordIterator<LINRecord const>(std::move(iter));
        }

        throw std::runtime_error("Not finalized?");
        return RecordIterator<LINRecord const>();
    }

}