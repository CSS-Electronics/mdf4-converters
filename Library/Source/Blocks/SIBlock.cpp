#include "SIBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    SIBlockBusType operator&(SIBlockBusType const &lhs, SIBlockBusType const &rhs) {
        using ut = std::underlying_type_t<SIBlockBusType>;

        ut intermediate = static_cast<ut>(lhs) & static_cast<ut>(rhs);

        return static_cast<SIBlockBusType>(intermediate);
    }

#pragma pack(push, 1)
    struct SIBlockData {
        be::little_uint8_buf_t type;
        be::little_uint8_buf_t bus_type;
        be::little_uint8_buf_t flags;
        be::little_uint8_buf_t reserved[5];
    };
#pragma pack(pop)

    SIBlockBusType SIBlock::getBusType() const {
        return static_cast<SIBlockBusType>(busType);
    }

    std::shared_ptr<MDBlock> SIBlock::getComment() const {
        return std::dynamic_pointer_cast<MDBlock>(links[2]);
    }

    bool SIBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        SIBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char *>(&rawData), sizeof(rawData));

        if (bytesRead != sizeof(rawData)) {
            return false;
        }

        type = rawData.type.value();
        busType = rawData.bus_type.value();
        flags = rawData.flags.value();

        result = true;

        return result;
    }

    bool SIBlock::saveBlockData(uint8_t *dataPtr) {
        auto ptr = reinterpret_cast<SIBlockData *>(dataPtr);

        ptr->type = type;
        ptr->bus_type = busType;
        ptr->flags = flags;

        return true;
    }

}
