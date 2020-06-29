#include "CNBlock.h"

#include "DTBlock.h"
#include "SDBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

#pragma pack(push, 1)
    struct CNBlockData {
        be::little_uint8_buf_t cn_type;
        be::little_uint8_buf_t cn_sync_type;
        be::little_uint8_buf_t cn_data_type;
        be::little_uint8_buf_t cn_bit_offset;
        be::little_uint32_buf_t cn_byte_offset;
        be::little_uint32_buf_t cn_bit_count;
        be::little_uint32_buf_t cn_flags;
        be::little_uint32_buf_t cn_inval_bit_pos;
        be::little_uint8_buf_t cn_precision;
        be::little_uint8_buf_t reserved;
        be::little_uint16_buf_t cn_attachment_count;
        be::little_float64_buf_t cn_val_range_min;
        be::little_float64_buf_t cn_val_range_max;
        be::little_float64_buf_t cn_limit_min;
        be::little_float64_buf_t cn_limit_max;
        be::little_float64_buf_t cn_limit_ext_min;
        be::little_float64_buf_t cn_limit_ext_max;
    };
#pragma pack(pop)

    uint8_t CNBlock::getBitOffset() const {
        return bitOffset;
    }

    uint32_t CNBlock::getByteOffset() const {
        return byteOffset;
    }

    uint32_t CNBlock::getBitCount() const {
        return bitCount;
    }

    CNType CNBlock::getChannelType() const {
        return CNType(type);
    }

    CNDataType CNBlock::getDataType() const {
        return CNDataType(dataType);
    }

    CNSyncType CNBlock::getSyncType() const {
        return CNSyncType(syncType);
    }

    std::shared_ptr<CNBlock> CNBlock::getNextCNBlock() const {
        return std::dynamic_pointer_cast<CNBlock>(links[0]);
    }

    std::shared_ptr<CNBlock> CNBlock::getCompositionBlock() const {
        return std::dynamic_pointer_cast<CNBlock>(links[1]);
    }

    std::shared_ptr<MdfBlock> CNBlock::getNameBlock() const {
        return links[2];
    }

    std::shared_ptr<MdfBlock> CNBlock::getSourceBlock() const {
        return links[3];
    }

    std::shared_ptr<MdfBlock> CNBlock::getDataBlock() const {
        return links[5];
    }

    void CNBlock::setDataBlock(std::shared_ptr<MdfBlock> block) {
        if (block) {
            if (block->getHeader().blockType == MdfBlockType_DT) {
                links[5] = std::dynamic_pointer_cast<DTBlock>(block);
            } else if (block->getHeader().blockType == MdfBlockType_SD) {
                links[5] = std::dynamic_pointer_cast<SDBlock>(block);
            } else {
                links[5] = block;
            }
        } else {
            links[5] = block;
        }
    }

    bool CNBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        CNBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char *>(&rawData), sizeof(rawData));

        if (bytesRead != sizeof(rawData)) {
            return false;
        }

        bitOffset = rawData.cn_bit_offset.value();
        byteOffset = rawData.cn_byte_offset.value();
        bitCount = rawData.cn_bit_count.value();
        flags = rawData.cn_flags.value();
        dataType = rawData.cn_data_type.value();
        syncType = rawData.cn_sync_type.value();
        type = rawData.cn_type.value();

        valRangeMin = rawData.cn_val_range_min.value();
        valRangeMax = rawData.cn_val_range_max.value();

        result = true;

        return result;
    }

    bool CNBlock::saveBlockData(uint8_t *dataPtr) {
        auto ptr = reinterpret_cast<CNBlockData *>(dataPtr);

        ptr->cn_type = type;
        ptr->cn_sync_type = syncType;
        ptr->cn_data_type = dataType;
        ptr->cn_bit_offset = bitOffset;
        ptr->cn_byte_offset = byteOffset;
        ptr->cn_bit_count = bitCount;
        ptr->cn_flags = flags;

        ptr->cn_val_range_min = valRangeMin;
        ptr->cn_val_range_max = valRangeMax;

        return true;
    }

}
