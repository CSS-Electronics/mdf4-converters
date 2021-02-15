#include "FinalizedFileInfo.h"
#include "FileInfoUtility.h"
#include "../Blocks/DTBlockMultipleRecordIDs.h"
#include "../Logger.h"
#include "../Blocks/DTBlockSingleContinuous.h"

#include <boost/dynamic_bitset.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>

namespace mdf {

    // Static helper functions.
    static long long findMatchingLocation(std::shared_ptr<DGBlock> dgBlock, std::vector<std::shared_ptr<CGBlock>> targets);

    FinalizedFileInfo::FinalizedFileInfo(std::shared_ptr<HDBlock> hdBlock, std::shared_ptr<std::streambuf> dataSource) :
        hdBlock(std::move(hdBlock)),
        dataSource(std::move(dataSource)) {
    }

    std::chrono::nanoseconds FinalizedFileInfo::firstMeasurement() const {
        std::chrono::nanoseconds result = std::chrono::nanoseconds::max();

        // Find all channel groups with time as master channel.
        std::vector<std::shared_ptr<CGBlock>> masterChannels;

        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        while(dgBlock) {
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

            while(cgBlock) {
                // Only use blocks with records.
                if(cgBlock->getCycleCount() != 0) {
                    // Extract the master from this group.
                    std::shared_ptr<CNBlock> masterBlock = getMasterTimeChannel(cgBlock);

                    if(masterBlock) {
                        masterChannels.push_back(cgBlock);
                    }
                }

                cgBlock = cgBlock->getNextCGBlock();
            }

            // Iterate to the next DG block in the chain.
            dgBlock = dgBlock->getNextDGBlock();
        }

        // Extract records from the relevant DT blocks until a record from one of the master channels is located.
        dgBlock = hdBlock->getFirstDGBlock();

        while(dgBlock) {
            // Get a list of all the CG blocks which are referenced from this DG block.
            std::vector<std::shared_ptr<CGBlock>> channelsInDGBlock;
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

            while(cgBlock) {
                if(std::find(masterChannels.begin(), masterChannels.end(), cgBlock) != masterChannels.end()) {
                    channelsInDGBlock.push_back(cgBlock);
                }

                cgBlock = cgBlock->getNextCGBlock();
            }

            do {
                if(channelsInDGBlock.empty()) {
                    // No target blocks in this DG block, skip.
                    break;
                }

                // Read records from the DT block until a matching block is found.
                long long location = findMatchingLocation(dgBlock, channelsInDGBlock);

                if(location == -1) {
                    // Could not find a matching DT block.
                    break;
                }

                // Read the record ID.
                uint64_t recordID = 0;
                auto recordLength = dgBlock->getRecordSize();
                std::shared_ptr<CGBlock> cgBlock_;
                dataSource->pubseekoff(location, std::ios_base::beg);

                // Find the matching CG block.
                if(recordLength != 0) {
                    dataSource->sgetn(reinterpret_cast<char*>(&recordID), recordLength);

                    for (auto block: masterChannels) {
                        if (block->getRecordID() == recordID) {
                            cgBlock_ = block;
                            break;
                        }
                    }
                } else {
                    // Only a single CG block.
                    for (auto block: masterChannels) {
                        if (block == dgBlock->getFirstCGBlock()) {
                            cgBlock_ = block;
                            break;
                        }
                    }
                }

                if(!cgBlock_) {
                    break;
                }

                std::shared_ptr<CNBlock> masterChannel = getMasterTimeChannel(cgBlock_);

                // Seek to the start byte for time data.
                dataSource->pubseekoff(masterChannel->getByteOffset(), std::ios_base::cur);

                // Determine how many bytes to read out.
                std::size_t bitsToRead = masterChannel->getBitCount() + masterChannel->getBitOffset();
                std::size_t bytesToRead = bitsToRead / 8;

                if(bitsToRead % 8 != 0) {
                    bytesToRead += 1;
                }

                uint8_t buffer[8];
                dataSource->sgetn(reinterpret_cast<char*>(buffer), bytesToRead);

                boost::dynamic_bitset<uint8_t> bitData(bytesToRead * 8);
                bitData.clear();
                bitData.init_from_block_range(buffer, buffer + bytesToRead);

                // Shift away un-needed data.
                bitData >>= masterChannel->getBitOffset();

                // Clip away un-needed data.
                bitData.resize(masterChannel->getBitCount());

                // Extract value.
                std::bitset<64> raw;
                for (unsigned i = 0; i < masterChannel->getBitCount(); ++i) {
                    raw[i] = bitData[i];
                }
                uint64_t valueRaw = raw.to_ullong();
                double value = *reinterpret_cast<double*>(&valueRaw);

                uint64_t ns = value;
                std::chrono::nanoseconds dur(ns);

                if(dur < result) {
                    result = dur;
                }
            } while(false);

            // Iterate to the next DG block in the chain.
            dgBlock = dgBlock->getNextDGBlock();
        }

        // Extract the start time from the HD block.
        result += static_cast<std::chrono::nanoseconds>(hdBlock->getStartTimeNs());

        return result;
    }

    static long long findMatchingLocation(std::shared_ptr<DGBlock> dgBlock, std::vector<std::shared_ptr<CGBlock>> targets) {
        long long result = -1;

        if(dgBlock->getFirstCGBlock()->getNextCGBlock()) {
            // Multiple CG blocks in the same DT block.
            auto dtBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(dgBlock->getDataBlock());

            if(!dtBlock) {
                std::cout << "Wrong block type" << std::endl;
                return result;
            }

            std::vector<uint64_t> targetIDs;

            for(auto const& target: targets) {
                targetIDs.emplace_back(target->getRecordID());
            }

            result = dtBlock->findFirstMatching(targetIDs);
        } else {
            // Sorted, only a single CG block in the DT block.
            //auto dtBlock = std::dynamic_pointer_cast<DTBlockSingleContinuous>(dgBlock->getDataBlock());

            result = dgBlock->getDataBlock()->getFileLocation();
        }

        return result;
    }

}
