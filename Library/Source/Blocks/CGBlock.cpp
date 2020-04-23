#include "CGBlock.h"

#include <boost/endian/buffers.hpp>

namespace mdf {

    CGBlockFlags operator&(CGBlockFlags const& lhs, CGBlockFlags const& rhs) {
        using ut = std::underlying_type_t<CGBlockFlags>;

        ut intermediate = static_cast<ut>(lhs) & static_cast<ut>(rhs);

        return static_cast<CGBlockFlags>(intermediate);
    }

    #pragma pack(push, 1)
    struct CGBlockData {
        boost::endian::little_uint64_buf_t record_id;
        boost::endian::little_uint64_buf_t cycle_count;
        boost::endian::little_uint16_buf_t flags;
        boost::endian::little_uint16_buf_t path_separator;
        boost::endian::little_uint8_buf_t reserved[4];
        boost::endian::little_uint32_buf_t data_bytes;
        boost::endian::little_uint32_buf_t inval_bytes;
    };
    #pragma pack(pop)

    uint64_t CGBlock::getRecordID() const {
        return recordID;
    }

    int64_t CGBlock::getRecordSize() const {
        return recordSize;
    }

    bool CGBlock::isVLSD() const {
        return flags & 0x0001u;
    }

    CGBlockFlags CGBlock::getFlags() const {
        return static_cast<CGBlockFlags>(flags);
    }

    void CGBlock::setVLSDDataBytes(uint64_t dataBytes) {
        this->dataBytes = dataBytes & 0x00000000FFFFFFFFull;
        this->invalDataBytes = (dataBytes & 0xFFFFFFFF00000000ull) >> 32u;
    }

    std::shared_ptr<CGBlock> CGBlock::getNextCGBlock() const {
        return std::dynamic_pointer_cast<CGBlock>(links[0]);
    }

    std::shared_ptr<CNBlock> CGBlock::getFirstCNBlock() const {
        return std::dynamic_pointer_cast<CNBlock>(links[1]);
    }

    std::shared_ptr<SIBlock> CGBlock::getSIBlock() const {
        return std::dynamic_pointer_cast<SIBlock>(links[3]);
    }

    bool CGBlock::load(uint8_t const* dataPtr) {
        bool result = false;

        // Load data into a struct for easier access.
        auto ptr = reinterpret_cast<CGBlockData const*>(dataPtr);

        cycleCount = ptr->cycle_count.value();
        dataBytes = ptr->data_bytes.value();
        invalDataBytes = ptr->inval_bytes.value();
        flags = ptr->flags.value();
        recordID = ptr->record_id.value();
        pathSeparator = ptr->path_separator.value();

        if(flags != 0x01) {
            // Calculate record size all from channels.
            std::shared_ptr<CNBlock> cnBlock = getFirstCNBlock();

            uint32_t largest = 0;

            while (cnBlock) {
                // Calculate the size of the current block, if not variable length.
                if (cnBlock->getChannelType() != CNType::VariableLengthData) {
                    uint32_t currentSize = 0;
                    currentSize += cnBlock->getByteOffset() * 8;
                    currentSize += cnBlock->getBitOffset();
                    currentSize += cnBlock->getBitCount();

                    if (currentSize > largest) {
                        largest = currentSize;
                    }
                }

                // Continue to the next block.
                cnBlock = cnBlock->getNextCNBlock();
            }

            // Convert from bits to bytes.
            recordSize = largest / 8;
            if (largest % 8 != 0) {
                recordSize += largest - (largest % 8);
            }
        } else {
            recordSize = -1;
        }

        result = true;

        return result;
    }

    void CGBlock::setCycleCount(uint64_t cycleCount) {
        this->cycleCount = cycleCount;
    }

    uint64_t CGBlock::getCycleCount() const {
        return cycleCount;
    }

    void CGBlock::setNextCGBlock(std::shared_ptr<CGBlock> block) {
        links[0] = block;
    }

    bool CGBlock::saveBlockData(uint8_t* dataPtr) {
        // Load data into a struct for easier access.
        auto ptr = reinterpret_cast<CGBlockData*>(dataPtr);

        ptr->record_id = recordID;
        ptr->cycle_count = cycleCount;
        ptr->flags = flags;
        ptr->path_separator = pathSeparator;
        ptr->data_bytes = dataBytes;
        ptr->inval_bytes = invalDataBytes;

        return true;
    }

}
