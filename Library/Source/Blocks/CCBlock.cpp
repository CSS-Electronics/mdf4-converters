#include "CCBlock.h"

#include <boost/endian/buffers.hpp>
#include <boost/endian/conversion.hpp>

namespace mdf {

    #pragma pack(push, 1)
    struct CCBlockData {
        boost::endian::little_uint8_buf_t cc_type;
        boost::endian::little_uint8_buf_t cc_precision;
        boost::endian::little_uint16_buf_t ch_flags;
        boost::endian::little_uint16_buf_t cc_ref_count;
        boost::endian::little_uint16_buf_t cc_val_count;
        boost::endian::little_float64_buf_t cc_phy_range_min;
        boost::endian::little_float64_buf_t cc_phy_range_max;
    };
    #pragma pack(pop)

    bool CCBlock::load(uint8_t const* dataPtr) {
        bool result = false;
        convertionValues.clear();

        // Load data into a struct for easier access.
        auto ptr = reinterpret_cast<CCBlockData const*>(dataPtr);

        type = ptr->cc_type.value();
        precision = ptr->cc_precision.value();
        flags = ptr->ch_flags.value();
        refCount = ptr->cc_ref_count.value();
        valueCount = ptr->cc_val_count.value();
        physicalRangeMinimum = ptr->cc_phy_range_min.value();
        physicalRangeMaximum = ptr->cc_phy_range_max.value();

        dataPtr += sizeof(CCBlockData);

        // Load any values used for convertion.
        for(uint64_t i = 0; i < valueCount; ++i) {
            auto value = reinterpret_cast<boost::endian::little_float64_buf_t const*>(dataPtr);
            convertionValues.emplace_back(value->value());
            dataPtr += sizeof(boost::endian::little_float64_buf_t);
        }

        result = true;

        return result;
    }

    bool CCBlock::saveBlockData(uint8_t* dataPtr) {

        auto ptr = reinterpret_cast<CCBlockData*>(dataPtr);

        ptr->cc_type = type;
        ptr->cc_precision = precision;
        ptr->ch_flags = flags;
        ptr->cc_ref_count = refCount;
        ptr->cc_val_count = valueCount;
        ptr->cc_phy_range_min = physicalRangeMinimum;
        ptr->cc_phy_range_max = physicalRangeMaximum;

        dataPtr += sizeof(CCBlockData);

        // Write convertion values.
        for(auto const& value: convertionValues) {
            auto v = reinterpret_cast<boost::endian::little_float64_buf_t*>(dataPtr);
            *v = value;
            dataPtr += sizeof(boost::endian::little_float64_buf_t);
        }

        return true;
    }

}
