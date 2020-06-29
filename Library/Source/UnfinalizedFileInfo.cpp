#include "UnfinalizedFileInfo.h"
#include "Blocks/DTBlockMultipleRecordIDs.h"
#include "Logger.h"

#include <boost/dynamic_bitset.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>

namespace mdf {

    // Static helper functions.
    std::shared_ptr<CNBlock> getMasterTimeChannel(std::shared_ptr<CGBlock> cgBlock);
    long long findMatchingLocation(std::shared_ptr<DGBlock> dgBlock, std::vector<std::shared_ptr<CGBlock>> targets);

    UnfinalizedFileInfo::UnfinalizedFileInfo(std::shared_ptr<HDBlock> hdBlock, std::shared_ptr<std::streambuf> dataSource) :
        hdBlock(std::move(hdBlock)),
        dataSource(std::move(dataSource)) {
    }

    std::chrono::nanoseconds UnfinalizedFileInfo::firstMeasurement() const {
        std::chrono::nanoseconds result;

        // Extract the start time from the HD block.
        result = static_cast<std::chrono::nanoseconds>(hdBlock->getStartTimeNs());

        // Find all channel groups with time as master channel.
        std::vector<std::shared_ptr<CGBlock>> masterChannels;

        std::shared_ptr<DGBlock> dgBlock = hdBlock->getFirstDGBlock();

        while(dgBlock) {
            std::shared_ptr<CGBlock> cgBlock = dgBlock->getFirstCGBlock();

            while(cgBlock) {
                // Extract the master from this group.
                std::shared_ptr<CNBlock> masterBlock = getMasterTimeChannel(cgBlock);

                if(masterBlock) {
                    masterChannels.push_back(cgBlock);
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

            // Read records from the DT block until a matching block is found.
            long long location = findMatchingLocation(dgBlock, channelsInDGBlock);

            do {
                if(location == -1) {
                    std::cout << "Break due to location" << std::endl;
                    break;
                }

                // Read the record ID.
                uint64_t recordID = 0;
                auto recordLength = dgBlock->getRecordSize();
                dataSource->pubseekoff(location, std::ios_base::beg);
                dataSource->sgetn(reinterpret_cast<char*>(&recordID), recordLength);

                // Find the matching CG block.
                std::shared_ptr<CGBlock> cgBlock_;
                for(auto block: masterChannels) {
                    if(block->getRecordID() == recordID) {
                        cgBlock_ = block;
                        break;
                    }
                }

                if(!cgBlock_) {
                    std::cout << "Break due to missing CG block" << std::endl;
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

                // TODO: If multiple DG are present, this needs to take those into account. Since this is not the
                //       case for CANedge, this is left for the future.
                result += dur;
            } while(false);

            // Iterate to the next DG block in the chain.
            dgBlock = dgBlock->getNextDGBlock();
        }

        return result;
    }

    long long findMatchingLocation(std::shared_ptr<DGBlock> dgBlock, std::vector<std::shared_ptr<CGBlock>> targets) {
        long long result = -1;

        std::shared_ptr<DTBlockMultipleRecordIDs> dtBlock = std::dynamic_pointer_cast<DTBlockMultipleRecordIDs>(dgBlock->getDataBlock());

        if(!dtBlock) {
            std::cout << "Wrong block type" << std::endl;
            return result;
        }

        std::vector<uint64_t> targetIDs;

        for(auto const& target: targets) {
            targetIDs.emplace_back(target->getRecordID());
        }

        result = dtBlock->findFirstMatching(targetIDs);

        return result;
    }

    std::shared_ptr<CNBlock> getMasterTimeChannel(std::shared_ptr<CGBlock> cgBlock) {
        std::shared_ptr<CNBlock> cnBlock = cgBlock->getFirstCNBlock();

        while(cnBlock) {
            bool resultFound = false;

            do {
                if((cnBlock->getChannelType() & CNType::MasterChannel) != CNType::MasterChannel) {
                    break;
                }

                if((cnBlock->getSyncType() & CNSyncType::Time) != CNSyncType::Time) {
                    break;
                }

                resultFound = true;
            } while(false);

            if(resultFound) {
                break;
            }

            // NOTE: No need to check composition blocks, as they cannot contain the master channel.
            cnBlock = cnBlock->getNextCNBlock();
        }

        // If a result was located, cnBlock is a valid pointer to the master channel. Else, it has no reference.
        return cnBlock;
    }

}
