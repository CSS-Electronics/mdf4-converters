#include "CGBlock.h"

#include <streambuf>

#include <boost/endian/buffers.hpp>

namespace be = boost::endian;

namespace mdf {

    CGBlockFlags operator&(CGBlockFlags const& lhs, CGBlockFlags const& rhs) {
        using ut = std::underlying_type_t<CGBlockFlags>;

        ut intermediate = static_cast<ut>(lhs) & static_cast<ut>(rhs);

        return static_cast<CGBlockFlags>(intermediate);
    }

    #pragma pack(push, 1)
    struct CGBlockData {
        be::little_uint64_buf_t record_id;
        be::little_uint64_buf_t cycle_count;
        be::little_uint16_buf_t flags;
        be::little_uint16_buf_t path_separator;
        be::little_uint8_buf_t reserved[4];
        be::little_uint32_buf_t data_bytes;
        be::little_uint32_buf_t inval_bytes;
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

    bool CGBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        CGBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char*>(&rawData), sizeof(rawData));

        if(bytesRead != sizeof(rawData)) {
            return false;
        }

        cycleCount = rawData.cycle_count.value();
        dataBytes = rawData.data_bytes.value();
        invalDataBytes = rawData.inval_bytes.value();
        flags = rawData.flags.value();
        recordID = rawData.record_id.value();
        pathSeparator = rawData.path_separator.value();

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
