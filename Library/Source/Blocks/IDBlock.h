#ifndef MDFSIMPLECONVERTERS_IDBLOCK_H
#define MDFSIMPLECONVERTERS_IDBLOCK_H

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

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

    struct IDBlock {
        bool load(std::shared_ptr<std::streambuf> stream);
        bool save(uint8_t* dataPtr);

        FinalizationFlags getFinalizationFlags() const;
        void setFinalizationFlags(FinalizationFlags flags);
    private:
        std::string creationTool;
        uint16_t versionNum;
        std::string versionStr;
        FinalizationFlags finalizationFlags;
        uint16_t customFinalizationFlags;
    };

}


#endif //MDFSIMPLECONVERTERS_IDBLOCK_H
