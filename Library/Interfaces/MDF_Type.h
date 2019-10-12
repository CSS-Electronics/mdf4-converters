#ifndef MDFSORTER_MDF_TYPE_H
#define MDFSORTER_MDF_TYPE_H

#include <cstdint>
#include <string>

/**
 * Create a raw byte representation of a string of up to 8 characters. Assumes the platform endianness is little
 * endian.
 *
 * @param value         String to convert.
 * @param position      Current position in the recursive call.
 * @return              The constructed byte value so far.
 */
uint64_t constexpr strToU64(char const* value, int position = 0) {
    uint64_t result = 0;

    if(*value != '\0') {
        result = *value << position * 8;
        result |= strToU64(value + 1, position + 1);
    }

    return result;
}

namespace mdf {

    enum MDF_Type : uint64_t {
        MDF_Type_Invalid = 0,
        MDF_Type_CC = strToU64("##CC"),
        MDF_Type_CG = strToU64("##CG"),
        MDF_Type_CN = strToU64("##CN"),
        MDF_Type_DG = strToU64("##DG"),
        MDF_Type_DT = strToU64("##DT"),
        MDF_Type_DZ = strToU64("##DZ"),
        MDF_Type_FH = strToU64("##FH"),
        MDF_Type_ID = 1,
        MDF_Type_HD = strToU64("##HD"),
        MDF_Type_MD = strToU64("##MD"),
        MDF_Type_SI = strToU64("##SI"),
        MDF_Type_SD = strToU64("##SD"),
        MDF_Type_TX = strToU64("##TX"),
    };

    std::string typeToString(MDF_Type type);

}

#endif //MDFSORTER_MDF_TYPE_H
