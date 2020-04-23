#include "IDBlock.h"

#include <algorithm>

namespace mdf {

    constexpr uint8_t const finalizedIdentifierStr[8] = {'M', 'D', 'F', ' ', ' ', ' ', ' ', ' '};
    constexpr uint8_t const unfinalizedIdentifierStr[8] = {'U', 'n', 'F', 'i', 'n', 'M', 'F', ' '};

    struct IDBlockData {
        uint8_t identifierStr[8];
        uint8_t versionStr[8];
        uint8_t toolStr[8];
        uint8_t reserved1[4];
        uint16_t versionNum;
        uint8_t reserved2[30];
        FinalizationFlags finalizationFlags;
        uint16_t customFinalizationFlags;
    };

    bool IDBlock::load(uint8_t const* dataPtr) {
        auto ptr = reinterpret_cast<IDBlockData const*>(dataPtr);

        // Determine if this is a MDF4 file at all.
        if(
            !std::equal(std::cbegin(ptr->identifierStr), std::cend(ptr->identifierStr), std::cbegin(finalizedIdentifierStr)) &&
            !std::equal(std::cbegin(ptr->identifierStr), std::cend(ptr->identifierStr), std::cbegin(unfinalizedIdentifierStr))
            ) {
            return false;
        }

        // Read out data.
        creationTool = std::string(reinterpret_cast<char const*>(ptr->toolStr), sizeof(ptr->toolStr));
        versionNum = ptr->versionNum;
        versionStr = std::string(reinterpret_cast<char const*>(ptr->versionStr), sizeof(ptr->versionStr));

        // Read any finalization flags.
        finalizationFlags = ptr->finalizationFlags;
        customFinalizationFlags = ptr->customFinalizationFlags;

        return true;
    }

    FinalizationFlags IDBlock::getFinalizationFlags() const {
        return finalizationFlags;
    }

    void IDBlock::setFinalizationFlags(mdf::FinalizationFlags flags) {
        this->finalizationFlags = flags;
    }

    bool IDBlock::save(uint8_t *dataPtr) {
        auto ptr = reinterpret_cast<IDBlockData*>(dataPtr);
        std::fill(dataPtr, dataPtr + sizeof(IDBlockData), 0x00);

        // Check the finalization flags.
        if((finalizationFlags == FinalizationFlags_None) && (customFinalizationFlags == 0)) {
            // Write as finalized.
            std::copy(std::cbegin(finalizedIdentifierStr), std::cend(finalizedIdentifierStr), ptr->identifierStr);
        } else {
            // Write as unfinalized.
            std::copy(std::cbegin(unfinalizedIdentifierStr), std::cend(unfinalizedIdentifierStr), ptr->identifierStr);
            ptr->finalizationFlags = finalizationFlags;
            ptr->customFinalizationFlags = customFinalizationFlags;
        }

        // Write tool and version information.
        ptr->versionNum = versionNum;
        std::copy(std::cbegin(versionStr), std::cend(versionStr), ptr->versionStr);
        std::copy(std::cbegin(creationTool), std::cend(creationTool), ptr->toolStr);

        return true;
    }

}