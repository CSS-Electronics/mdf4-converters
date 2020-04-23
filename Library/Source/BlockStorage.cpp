#include "BlockStorage.h"
#include "Blocks/MdfBlock.h"

#include <cstring>
#include <set>

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

    BlockStorage::BlockStorage(mio::shared_mmap_source mmap) : mmap(std::move(mmap)) {

    }

    std::shared_ptr<MdfBlock> BlockStorage::getBlockAt(uint64_t address) {
        std::shared_ptr<MdfBlock> result;

        // Is this block already loaded.
        auto iter = blockMap.left.find(address);
        if(iter == std::end(blockMap.left)) {
            uint8_t const* dataPtr = reinterpret_cast<uint8_t const*>(mmap.data()) + address;

            // Attempt to read the header at the address.
            MdfHeader header;
            static_assert(sizeof(MdfHeader) == 3 * sizeof(uint64_t));
            std::memcpy(&header, dataPtr, sizeof(header));
            dataPtr += sizeof(header);

            // TODO: Only continue if the header is valid.

            // Load all the links.
            std::vector<std::shared_ptr<MdfBlock>> links(header.linkCount);
            for (uint64_t linkNumber = 0; linkNumber < header.linkCount; linkNumber++) {
                auto linkLocation = reinterpret_cast<uint64_t const*>(dataPtr);

                if (*linkLocation != 0) {
                    links[linkNumber] = getBlockAt(*linkLocation);
                }

                dataPtr += sizeof(uint64_t);
            }

            // Construct the block at this location, and store it.
            result = createBlock(header, links, dataPtr);

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
