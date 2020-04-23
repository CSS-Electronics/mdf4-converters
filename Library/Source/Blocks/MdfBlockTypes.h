#ifndef MDFSIMPLECONVERTERS_MDFBLOCKTYPES_H
#define MDFSIMPLECONVERTERS_MDFBLOCKTYPES_H

#include <cstdint>
#include <string>

namespace mdf {

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

        if (*value != '\0') {
            result = *value << position * 8;
            result |= strToU64(value + 1, position + 1);
        }

        return result;
    }


    enum MdfBlockType : uint64_t {
        MdfBlockType_Invalid = 0,
        MdfBlockType_CC = strToU64("##CC"),
        MdfBlockType_CG = strToU64("##CG"),
        MdfBlockType_CN = strToU64("##CN"),
        MdfBlockType_DG = strToU64("##DG"),
        MdfBlockType_DT = strToU64("##DT"),
        MdfBlockType_DZ = strToU64("##DZ"),
        MdfBlockType_FH = strToU64("##FH"),
        MdfBlockType_HD = strToU64("##HD"),
        MdfBlockType_MD = strToU64("##MD"),
        MdfBlockType_SI = strToU64("##SI"),
        MdfBlockType_SD = strToU64("##SD"),
        MdfBlockType_TX = strToU64("##TX"),
    };

    std::string typeToString(MdfBlockType type);

}

#endif //MDFSIMPLECONVERTERS_MDFBLOCKTYPES_H
