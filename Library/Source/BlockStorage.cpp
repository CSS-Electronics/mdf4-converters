#include "BlockStorage.h"
#include "Blocks/MdfBlock.h"

#include <cstring>
#include <streambuf>
#include <set>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    bool MdfBlockSortingOrder::operator()(const std::shared_ptr<MdfBlock> &lhs, const std::shared_ptr<MdfBlock> &rhs) const {
        // Always place HD blocks first.
        if(lhs->getHeader().blockType == MdfBlockType_HD) {
            return true;
        } else if(rhs->getHeader().blockType == MdfBlockType_HD) {
            return false;
        }

        // Prefer the original file position.
        if(lhs->getFileLocation() != rhs->getFileLocation()) {
            return lhs->getFileLocation() < rhs->getFileLocation();
        }

        // Pointer comparison.
        return lhs.get() < rhs.get();
    }

    BlockStorage::BlockStorage(std::shared_ptr<std::streambuf> stream) : stream(stream) {

    }

    std::shared_ptr<MdfBlock> BlockStorage::getBlockAt(uint64_t address) {
        std::shared_ptr<MdfBlock> result;

        // Is this block already loaded.
        auto iter = blockMap.left.find(address);
        if(iter == std::end(blockMap.left)) {
            // Attempt to read the header at the address.
            char buffer[sizeof(MdfHeader)] = { 0 };
            std::streampos streamLocation = stream->pubseekpos(address);

            if(streamLocation != address) {
                throw std::runtime_error("Could not seek to header");
            }

            std::streamsize bytesRead = stream->sgetn(buffer, sizeof(buffer));

            if(bytesRead != sizeof(buffer)) {
                throw std::runtime_error("Could not read enough bytes to fill header");
            }

            // Read into header structure.
            MdfHeader header;
            std::memcpy(&header, buffer, sizeof(header));

            // TODO: Only continue if the header is valid.

            // Load all the links.
            std::vector<std::shared_ptr<MdfBlock>> links(header.linkCount);
            for (uint64_t linkNumber = 0; linkNumber < header.linkCount; linkNumber++) {
                be::little_uint64_at linkLocation = 0;

                bytesRead = stream->sgetn(reinterpret_cast<char *>(linkLocation.data()), sizeof(linkLocation));
                if(bytesRead != sizeof(linkLocation)) {
                    throw std::runtime_error("Could not load link");
                }

                // Skip unset links.
                if(linkLocation == 0) {
                    continue;
                }

                // Recursive loading. Since this seeks to a new position, store the current location.
                auto position = stream->pubseekoff(0, std::ios_base::cur);
                links[linkNumber] = getBlockAt(linkLocation);
                stream->pubseekpos(position);
            }

            // Construct the block at this location, and store it.
            result = createBlock(header, links, stream);

            if (result) {
                // If any blocks were redefined, replace them in the map.
                auto newLinks = result->getLinks();

                for(std::size_t i = 0; i < links.size(); ++i) {
                    if(links[i] != newLinks[i]) {
                        // A block has been replaced. Find the old block in the list and replace it.
                        auto iterInner = blockMap.right.find(links[i]);

                        if(iterInner != std::end(blockMap.right)) {
                            blockMap.right.replace_key(iterInner, newLinks[i]);
                        }
                    }
                }

                blockMap.left.insert(std::make_pair(address, result));
            }
        } else {
            return iter->second;
        }

        return result;
    }

    const boost::bimap<uint64_t, std::shared_ptr<MdfBlock>> & BlockStorage::getBlockMap() const {
        return blockMap;
    }

    std::map<uint64_t, std::shared_ptr<MdfBlock> > BlockStorage::getPackedBlockMap() {
        // Starting with the HD block at location 64, get all linked blocks.
        auto hdBlock = getBlockAt(64);
        auto linkedBlocks = getLinkedBlocks(hdBlock);

        // Create a map, starting with the HD block, putting each block at the next available slot.
        std::map<uint64_t, std::shared_ptr<MdfBlock>> result;
        uint64_t nextPosition = 64;

        result.insert(std::make_pair(nextPosition, hdBlock));
        nextPosition += hdBlock->getHeader().blockSize;

        for(auto const& block: linkedBlocks) {
            if(block == hdBlock) {
                continue;
            }

            result.insert(std::make_pair(nextPosition, block));
            nextPosition += block->getHeader().blockSize;

            // Ensure boundaries between blocks are 64 bit aligned.
            if(nextPosition % sizeof(uint64_t) != 0) {
                nextPosition += sizeof(uint64_t) - nextPosition % sizeof(uint64_t);
            }
        }

        return result;
    }

    std::set<std::shared_ptr<MdfBlock>, MdfBlockSortingOrder> BlockStorage::getLinkedBlocks(std::shared_ptr<MdfBlock> startBlock) {
        std::set<std::shared_ptr<MdfBlock>, MdfBlockSortingOrder> result = std::set<std::shared_ptr<MdfBlock>, MdfBlockSortingOrder>();

        if(!startBlock) {
            return result;
        }

        // Add this block.
        result.insert(startBlock);

        // Add all dependent blocks.
        for(auto const& block: startBlock->getLinks()) {
            if(!block) {
                continue;
            }

            if(block->getHeader().blockType == MdfBlockType_SD) {
                int i = 5;
            }

            if(result.find(block) == std::end(result)) {
                result.merge(getLinkedBlocks(block));
            }
        }

        return result;
    }
}
