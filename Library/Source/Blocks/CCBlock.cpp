#include "CCBlock.h"

#include <streambuf>
#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    #pragma pack(push, 1)
    struct CCBlockData {
        be::little_uint8_buf_t cc_type;
        be::little_uint8_buf_t cc_precision;
        be::little_uint16_buf_t ch_flags;
        be::little_uint16_buf_t cc_ref_count;
        be::little_uint16_buf_t cc_val_count;
        be::little_float64_buf_t cc_phy_range_min;
        be::little_float64_buf_t cc_phy_range_max;
    };
    #pragma pack(pop)

    bool CCBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;
        convertionValues.clear();

        // Load data into a struct for easier access.
        CCBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char*>(&rawData), sizeof(rawData));

        if(bytesRead != sizeof(rawData)) {
            return false;
        }

        // Read out data, taking endian into consideration.
        type = rawData.cc_type.value();
        precision = rawData.cc_precision.value();
        flags = rawData.ch_flags.value();
        refCount = rawData.cc_ref_count.value();
        valueCount = rawData.cc_val_count.value();
        physicalRangeMinimum = rawData.cc_phy_range_min.value();
        physicalRangeMaximum = rawData.cc_phy_range_max.value();

        // CC block may have additional data used for conversion rules.
        // NOTE: For optimal performance, allocate an array and read entire block. Since the number potentially
        //       could be very large, it is just done in a loop.
        for(uint64_t i = 0; i < valueCount; ++i) {
            be::little_float64_buf_t value;
            bytesRead = stream->sgetn(reinterpret_cast<char*>(&value), sizeof(value));

            if(bytesRead != sizeof(value)) {
                return false;
            }

            convertionValues.emplace_back(value.value());

        }

        result = true;

        return result;
    }

    bool CCBlock::saveBlockData(std::streambuf *stream) {
        CCBlockData data;
        memset(&data, 0x00, sizeof(data));

        data.cc_type = type;
        data.cc_precision = precision;
        data.ch_flags = flags;
        data.cc_ref_count = refCount;
        data.cc_val_count = valueCount;
        data.cc_phy_range_min = physicalRangeMinimum;
        data.cc_phy_range_max = physicalRangeMaximum;

        std::streamsize written = stream->sputn(reinterpret_cast<const char *>(&data), sizeof(data));

        // Write conversion values.
        for(auto const& value: convertionValues) {
            boost::endian::little_float64_buf_t v(value);

            written += stream->sputn(reinterpret_cast<const char *>(&v), sizeof(v));
        }

        return (written == sizeof(data) * sizeof(boost::endian::little_float64_buf_t) * convertionValues.size());
    }

}
