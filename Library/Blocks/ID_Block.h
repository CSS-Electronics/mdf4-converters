#ifndef MDFSORTER_ID_BLOCK_H
#define MDFSORTER_ID_BLOCK_H

#include "MDF_Block.h"

namespace mdf {

    enum FinalizationFlags: uint16_t {
        FinalizationFlags_None = 0,
        FinalizationFlags_UpdateCycleCounterInCGCA = 0x01u << 0u,
        FinalizationFlags_UpdateCycleCounterInSR = 0x01u << 1u,
        FinalizationFlags_UpdateLengthInLastDT = 0x01u << 2u,
        FinalizationFlags_UpdateLengthInLastRD = 0x01u << 3u,
        FinalizationFlags_UpdateDL = 0x01u << 4u,
        FinalizationFlags_UpdateByteCountInVLSDCG = 0x01u << 5u,
        FinalizationFlags_UpdateOffsetsInVLSDCG = 0x01u << 6u,
    };

    inline FinalizationFlags& operator &=(FinalizationFlags& lhs, int rhs) {
        using EnumType = std::underlying_type_t<FinalizationFlags>;
        auto tempLhs = static_cast<EnumType>(lhs);
        auto tempRhs = static_cast<EnumType>(rhs);
        tempLhs = tempLhs & tempRhs;
        lhs = static_cast<FinalizationFlags>(tempLhs);
        return lhs;
    }

    struct ID_Data {
        uint8_t identifierStr[8];
        uint8_t versionStr[8];
        uint8_t toolStr[8];
        uint8_t reserved1[4];
        uint16_t versionNum;
        uint8_t reserved2[30];
        FinalizationFlags finalizationFlags;
        uint16_t customFinalizationFlags;
    };

    struct ID_Block : public MDF_Block {
        // Constructors.
        ID_Block();

        // Inherited functions.
        bool load(std::istream& stream) override;
        void save(std::ostream& stream) override;

        // Getters.
        // Setters.

        FinalizationFlags getFinalizationFlags() const;
        void setFinalizationFlags(FinalizationFlags& flags);
        ID_Data data;
    private:
    };

}

#endif //MDFSORTER_ID_BLOCK_H
