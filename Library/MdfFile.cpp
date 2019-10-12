#include "MdfFile.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <Exceptions/MDF_MissingBlockError.h>

#include "boost/range/adaptor/map.hpp"
#include "boost/range/adaptor/reversed.hpp"

#include "BlockTree.h"
#include "ExtractMetaData.h"
#include "FileInfo.h"
#include "Loader.h"
#include "Blocks/SD_Block.h"
#include "Iterators/CAN_DataFrame_Iterator.h"
#include "Iterators/CAN_DataFrame_Mapping.h"
#include "Iterators/LIN_Frame_Iterator.h"
#include "Iterators/LIN_Frame_Mapping.h"

namespace mdf {

    MdfFile::MdfFile(std::string const &fileName) : fileName(fileName) {
        // Open the file.
        inputStream.open(fileName, std::fstream::binary | std::fstream::in);

        // Create a loader to handle loading operations.
        Loader loader(inputStream);

        // Attempt to load the ID block.
        std::shared_ptr<MDF_Block> id = loader.loadBlock(0);
        if(!id) {
            throw MDF_MissingBlockError("Could not load required ID block, is it a valid MDF file?");
        }
        ID = std::dynamic_pointer_cast<ID_Block>(id);

        // Attempt to load the HD block.
        std::shared_ptr<MDF_Block> hd = loader.loadBlock(64);
        if(!hd) {
            throw MDF_MissingBlockError("Could not load required HD block, is it a valid MDF4 file?");
        }
        HD = std::dynamic_pointer_cast<HD_Block>(hd);

        // With all linked blocks loaded, start creating a tree. Insert the ID block as root and HD as first child.
        tree = std::make_shared<BlockTree>(ID, HD);
    }

    void MdfFile::finalize() {
        // Check the finalization flags.
        FinalizationFlags finalizationFlags = ID->getFinalizationFlags();

        if(finalizationFlags == FinalizationFlags_None) {
            return;
        }

        // Handle the DT block length first.
        if(finalizationFlags & FinalizationFlags_UpdateLengthInLastDT) {
            // Create a map of all blocks and their corresponding file location.
            std::map<uint64_t, std::shared_ptr<MDF_Block>> locationMap;

            for(std::shared_ptr<MDF_Block> const& block: tree->uniques) {
                locationMap.insert(std::make_pair(block->fileLocation, block));
            }

            std::shared_ptr<DT_Block> lastDTBlock;

            // Find the last DT block.
            for(std::shared_ptr<MDF_Block> const& block: locationMap | boost::adaptors::reversed | boost::adaptors::map_values) {
                if(block->header.type == MDF_Type_DT) {
                    lastDTBlock = std::dynamic_pointer_cast<DT_Block>(block);
                    break;
                }
            }

            // Determine if the DT size calculation is based on the file size, or the next block.
            auto iter = locationMap.find(lastDTBlock->fileLocation);
            std::advance(iter, 1);

            uint64_t endPosition = 0;
            if(iter == locationMap.cend()) {
                // Use file size.
                // Get the size of the file (Seek to end, get location).
                inputStream.seekg(0, std::fstream::end);
                endPosition = inputStream.tellg();
            } else {
                // Use block size.
                endPosition = iter->second->fileLocation;
            }

            // Calculate the size of the DT block.
            uint64_t blockSize = endPosition - lastDTBlock->fileLocation;
            lastDTBlock->header.size = blockSize;

            // Remove the flag.
            finalizationFlags &= ~FinalizationFlags_UpdateLengthInLastDT;
            ID->setFinalizationFlags(finalizationFlags);
        }

        // Next, handle cycle counters and data bytes.
        if((finalizationFlags & FinalizationFlags_UpdateCycleCounterInCGCA) || (finalizationFlags & FinalizationFlags_UpdateByteCountInVLSDCG)) {
            // Loop over each DG and contained CG.
            for(auto& dg: HD->dg) {
                std::map<uint64_t, uint64_t> cycleCounterResults;
                std::map<uint64_t, uint64_t> cycleCounterSize;

                std::shared_ptr<DT_Block> dt = std::dynamic_pointer_cast<DT_Block>(dg->blockLinks[2]);

                // Begin looping over the data.
                inputStream.seekg(dt->fileLocation + sizeof(dt->header));
                std::shared_ptr<GenericDataRecord> record = dt->getNextRecord(inputStream);

                while(record) {
                    // Update data.
                    cycleCounterResults[record->recordID] += 1;
                    cycleCounterSize[record->recordID] += record->data.size();

                    // Get next record.
                    record = dt->getNextRecord(inputStream);
                }

                // Update the CG blocks.
                for(auto& cg: dg->cg) {
                    uint64_t recordID = cg->data.record_id;
                    cg->data.cycle_count = cycleCounterResults[recordID];

                    if(cg->data.flags & 0x01) {
                        cg->data.data_bytes = cycleCounterSize[recordID];
                    }
                }
            }

            // Remove the flag.
            finalizationFlags &= ~FinalizationFlags_UpdateCycleCounterInCGCA;
            finalizationFlags &= ~FinalizationFlags_UpdateByteCountInVLSDCG;
            ID->setFinalizationFlags(finalizationFlags);
        }

        // Check that all finalization flags are cleared.
        if(finalizationFlags == FinalizationFlags_None) {
            // Correct text in ID block to finalized.
            uint8_t finalizedIdentifierStr[8] = { 'M', 'D', 'F', ' ', ' ', ' ', ' ', ' '};
            memcpy(ID->data.identifierStr, finalizedIdentifierStr, sizeof(finalizedIdentifierStr));
        }
    }

    void MdfFile::sort() {
        // Requires the file is finalized.
        finalize();

        // Create a copy of the entire tree.
        std::shared_ptr<ID_Block> id = std::make_shared<ID_Block>(*ID);
        std::shared_ptr<HD_Block> hd = std::make_shared<HD_Block>(*HD);

        std::shared_ptr<BlockTree> alt = std::make_shared<BlockTree>(id, hd);

        // First convert CG VLSD blocks to SD blocks.
        hd->getFirstDG()->convertCGVLtoSD();

        // Convert the unsorted DG blocks to sorted DG blocks.
        hd->sortDG();

        // Create a new tree with the added blocks.
        std::shared_ptr<BlockTree> alt2 = std::make_shared<BlockTree>(id, hd);

        // Pack the blocks and update their file locations.
        auto MdfTypeCustomSorter = [](std::shared_ptr<MDF_Block> const& left, std::shared_ptr<MDF_Block> const& right) {
            bool result = false;

            MDF_Type const& leftType = left->header.type;
            MDF_Type const& rightType = right->header.type;

            // Handle ID blocks.
            if((leftType == MDF_Type_ID) || (rightType == MDF_Type_ID)) {
                return leftType == MDF_Type_ID;
            }

            // Handle HD blocks.
            if((leftType == MDF_Type_HD) || (rightType == MDF_Type_HD)) {
                return leftType == MDF_Type_HD;
            }

            if(left->fileLocation == right->fileLocation) {
                // Fallback to pointer comparison
                result = left < right;
            } else {
                // Prefer static blocks first.
                result = left->fileLocation < right->fileLocation;
            }

            return result;
        };

        // Generate a map of all linked blocks.
        std::set<std::shared_ptr<MDF_Block>, decltype(MdfTypeCustomSorter)> linkedBlocks(MdfTypeCustomSorter);

        std::copy(alt2->uniques.cbegin(), alt2->uniques.cend(), std::inserter(linkedBlocks, linkedBlocks.begin()));

        // Sanity check.
        if(linkedBlocks.size() != alt2->uniques.size()) {
            throw;
        }

        // Update the file locations.
        uint64_t location = 0;
        for(auto& block: linkedBlocks) {
            block->fileLocation = location;
            location += block->header.size;

            // Handle alignment issues.
            auto alignmentError = location % 8;

            if(alignmentError != 0) {
                location += 8 - alignmentError;
            }
        }

        std::shared_ptr<BlockTree> alt3 = std::make_shared<BlockTree>(id, hd);

        // Replace the original ID, HD block and tree.
        ID = id;
        HD = hd;
        tree = alt3;
    }

    void MdfFile::save(std::string const &outputFileName) {
        // Create an output stream.
        std::fstream outStream(outputFileName, std::fstream::binary | std::fstream::out);

        auto MdfTypeCustomSorter = [](std::shared_ptr<MDF_Block> const& left, std::shared_ptr<MDF_Block> const& right) {
            bool result = false;

            MDF_Type const& leftType = left->header.type;
            MDF_Type const& rightType = right->header.type;

            // Handle ID blocks.
            if((leftType == MDF_Type_ID) || (rightType == MDF_Type_ID)) {
                return leftType == MDF_Type_ID;
            }

            // Handle HD blocks.
            if((leftType == MDF_Type_HD) || (rightType == MDF_Type_HD)) {
                return leftType == MDF_Type_HD;
            }

            if(left->fileLocation == right->fileLocation) {
                // Fallback to pointer comparison
                result = left < right;
            } else {
                // Prefer static blocks first.
                result = left->fileLocation < right->fileLocation;
            }

            return result;
        };

        // Generate a map of all linked blocks.
        std::set<std::shared_ptr<MDF_Block>, decltype(MdfTypeCustomSorter)> linkedBlocks(MdfTypeCustomSorter);

        std::copy(tree->uniques.cbegin(), tree->uniques.cend(), std::inserter(linkedBlocks, linkedBlocks.begin()));

        for(auto& block: linkedBlocks) {
            // Seek to the correct position.
            outStream.seekp(block->fileLocation);
            block->save(outStream);
        }

    }

    std::shared_ptr<CG_Block> MdfFile::findCANData() const {
        std::shared_ptr<CG_Block> result;

        // Find the DG block which contains CAN data.
        for(std::shared_ptr<DG_Block> const& dgBlock: HD->dg) {
            std::shared_ptr<CG_Block> cgBlock = dgBlock->getFirstCG();

            // Ensure it contains bus events.
            if(!(cgBlock->data.flags & 0x01 << 1)) {
                continue;
            }

            // Ensure it contains CAN data (Look at the source information).
            std::string sourceInformation = cgBlock->getAcquisitionSource()->getName()->getText();

            if(sourceInformation.find("CAN") == std::string::npos) {
                continue;
            }

            result = cgBlock;
            break;
        }

        return result;
    }

    std::shared_ptr<CG_Block> MdfFile::findLINData() const {
        std::shared_ptr<CG_Block> result;

        // Find the DG block which contains LIN data.
        for(std::shared_ptr<DG_Block> const& dgBlock: HD->dg) {
            for(std::shared_ptr<CG_Block> cgBlock: dgBlock->cg) {
                // Ensure it contains bus events.
                if(!(cgBlock->data.flags & 0x01 << 1)) {
                    continue;
                }

                // Ensure it contains CAN data (Look at the source information).
                std::string sourceInformation = cgBlock->getAcquisitionSource()->getName()->getText();

                if(sourceInformation.find("LIN") == std::string::npos) {
                    continue;
                }

                result = cgBlock;
                break;
            }
        }

        return result;
    }

    std::shared_ptr<CAN_DataFrame_Iterator> MdfFile::getCANDataFrameIterator() {
        std::shared_ptr<CAN_DataFrame_Iterator> result;

        // Find the DG block which contains CAN data.
        std::shared_ptr<CG_Block> cgBlock = findCANData();

        if(cgBlock) {
            // Find enclosing DG block.
            std::shared_ptr<DG_Block> dgBlock;

            for(std::shared_ptr<DG_Block> currentDG: HD->dg) {
                if(currentDG->getFirstCG() != cgBlock) {
                    continue;
                }

                dgBlock = currentDG;
            }

            if(!dgBlock) {
                // TODO: Raise error.
            }

            // Get data source.
            std::shared_ptr<DT_Block> dtBlock = dgBlock->getDataBlock();
            std::shared_ptr<SD_Block> sdBlock;

            // Generate mapping of the data.
            CANMappingTable mapping;

            std::shared_ptr<CN_Block> timeChannel;
            std::shared_ptr<CN_Block> dataChannel;

            // Find the time and data channels. Time must be the master channel.
            for(std::shared_ptr<CN_Block> const& cnBlock: cgBlock->cn) {
                if((cnBlock->data.cn_type == 2) && (cnBlock->data.cn_sync_type == 1)) {
                    timeChannel = cnBlock;
                } else {
                    dataChannel = cnBlock;
                }
            }

            // Create a list of all the channels, to perform proper mapping.
            std::vector<std::shared_ptr<CN_Block>> cnBlocks;
            cnBlocks.push_back(timeChannel);
            std::copy(dataChannel->composite.cbegin(), dataChannel->composite.cend(), std::back_inserter(cnBlocks));

            // Loop over all the channels, and extract the bit and byte offsets for each field.
            for(std::shared_ptr<CN_Block> const& cnBlock: cnBlocks) {
                assert(cnBlock);

                // Extract the name and attempt to get a relevant mapping from it.
                std::string channelName = cnBlock->getName()->getText();
                auto index = CAN_DataFrame_FieldMapping.find(channelName);

                // If the mapping yielded a valid index, store the offset information.
                if(index != CAN_DataFrame_FieldMapping.end()) {
                    mapping.emplace(index->second,
                                    std::make_tuple(
                                        cnBlock->data.cn_byte_offset,
                                        cnBlock->data.cn_bit_offset,
                                        cnBlock->data.cn_bit_count,
                                        cnBlock->data.cn_data_type,
                                        cnBlock->getDataSD()
                                    ));
                }
            }

            // Generate a new data iterator.
            result = std::make_shared<CAN_DataFrame_Iterator>(dtBlock, mapping, HD->getStartTime());
        } else {
            // TODO: Raise error.
        }

        return result;
    }

    std::shared_ptr<CAN_DataFrame_Iterator> MdfFile::getCANDataFrameIteratorEnd() {
        return std::make_shared<CAN_DataFrame_Iterator>();
    }

    std::shared_ptr<LIN_Frame_Iterator> MdfFile::getLINFrameIterator() {
        std::shared_ptr<LIN_Frame_Iterator> result;

        // Find the DG block which contains LIN data.
        std::shared_ptr<CG_Block> cgBlock = findLINData();

        if(cgBlock) {
            // Find enclosing DG block.
            std::shared_ptr<DG_Block> dgBlock;

            for(std::shared_ptr<DG_Block> currentDG: HD->dg) {
                if(currentDG->getFirstCG() != cgBlock) {
                    continue;
                }

                dgBlock = currentDG;
            }

            if(!dgBlock) {
                // TODO: Raise error.
            }

            // Get data source.
            std::shared_ptr<DT_Block> dtBlock = dgBlock->getDataBlock();
            std::shared_ptr<SD_Block> sdBlock;

            // Generate mapping of the data.
            LINMappingTable mapping;

            std::shared_ptr<CN_Block> timeChannel;
            std::shared_ptr<CN_Block> dataChannel;

            // Find the time and data channels. Time must be the master channel.
            for(std::shared_ptr<CN_Block> const& cnBlock: cgBlock->cn) {
                if((cnBlock->data.cn_type == 2) && (cnBlock->data.cn_sync_type == 1)) {
                    timeChannel = cnBlock;
                } else {
                    dataChannel = cnBlock;
                }
            }

            // Create a list of all the channels, to perform proper mapping.
            std::vector<std::shared_ptr<CN_Block>> cnBlocks;
            cnBlocks.push_back(timeChannel);
            std::copy(dataChannel->composite.cbegin(), dataChannel->composite.cend(), std::back_inserter(cnBlocks));

            // Loop over all the channels, and extract the bit and byte offsets for each field.
            for(std::shared_ptr<CN_Block> const& cnBlock: cnBlocks) {
                assert(cnBlock);

                // Extract the name and attempt to get a relevant mapping from it.
                std::string channelName = cnBlock->getName()->getText();
                auto index = LIN_Frame_FieldMapping.find(channelName);

                // If the mapping yielded a valid index, store the offset information.
                if(index != LIN_Frame_FieldMapping.end()) {
                    mapping.emplace(index->second,
                                    std::make_tuple(
                                        cnBlock->data.cn_byte_offset,
                                        cnBlock->data.cn_bit_offset,
                                        cnBlock->data.cn_bit_count,
                                        cnBlock->data.cn_data_type,
                                        cnBlock->getDataSD()
                                    ));
                }
            }

            // Generate a new data iterator.
            result = std::make_shared<LIN_Frame_Iterator>(dtBlock, mapping, HD->getStartTime());
        } else {
            // TODO: Raise error.
        }

        return result;
    }

    std::shared_ptr<LIN_Frame_Iterator> MdfFile::getLINFrameIteratorEnd() {
        return std::make_shared<LIN_Frame_Iterator>();
    }

    FileInfo_t MdfFile::getFileInfo() const {
        FileInfo_t fileInfo;
        memset(&fileInfo, 0x00, sizeof(fileInfo));

        // Extract the base shared info (If present).
        std::shared_ptr<MD_Block> metaData = HD->getComment();
        if(metaData) {
            std::string xmlDataShared = metaData->getData();
            getSharedInfo(fileInfo, xmlDataShared);
        }

        // Insert the boot time.
        fileInfo.Time = static_cast<time_t>(HD->getStartTime() * 1E-9);
        fileInfo.TimezoneOffset = static_cast<time_t>(HD->getTimezoneOffset() * 60);

        // Extract CAN data.
        std::shared_ptr<CG_Block> cgBlockCAN = findCANData();
        if(cgBlockCAN) {
            fileInfo.CANMessages = cgBlockCAN->data.cycle_count;
        }

        std::shared_ptr<SI_Block> siBlock = cgBlockCAN->getAcquisitionSource();
        if(siBlock) {
            std::string xmlDataCAN = siBlock->getComment()->getData();

            getCANInfo(fileInfo, xmlDataCAN);
        }

        // Extract LIN data.
        std::shared_ptr<CG_Block> cgBlockLIN = findLINData();
        if(cgBlockLIN) {
            fileInfo.LINMessages = cgBlockLIN->data.cycle_count;
        }

        return fileInfo;
    }

}
