#include "IDBlock.h"

#include <algorithm>
#include <streambuf>

#include <boost/endian/buffers.hpp>

namespace be = boost::endian;

namespace mdf {

    constexpr uint8_t const finalizedIdentifierStr[8] = {'M', 'D', 'F', ' ', ' ', ' ', ' ', ' '};
    constexpr uint8_t const unfinalizedIdentifierStr[8] = {'U', 'n', 'F', 'i', 'n', 'M', 'F', ' '};

    struct IDBlockData {
        uint8_t identifierStr[8];
        uint8_t versionStr[8];
        uint8_t toolStr[8];
        uint8_t reserved1[4];
        be::little_uint16_buf_t versionNum;
        uint8_t reserved2[30];
        be::little_uint16_buf_t finalizationFlags;
        be::little_uint16_buf_t customFinalizationFlags;
    };

    bool IDBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        do {
            std::streampos streamLocation = stream->pubseekpos(0);
            if(streamLocation != 0) {
                throw std::runtime_error("Could not seek to ID block header");
            }

            char buffer[sizeof(IDBlockData)] = { 0 };
            std::streamsize bytesRead = stream->sgetn(buffer, sizeof(buffer));

            if(bytesRead != sizeof(buffer)) {
                throw std::runtime_error("Could not read enough bytes to fill ID block");
            }

            // Treat the buffer as a block of data with the correct endianess.
            IDBlockData const* const data = reinterpret_cast<IDBlockData const*>(buffer);

            // Determine if this is a MDF4 file at all.
            if(
                !std::equal(std::cbegin(data->identifierStr), std::cend(data->identifierStr), std::cbegin(finalizedIdentifierStr)) &&
                !std::equal(std::cbegin(data->identifierStr), std::cend(data->identifierStr), std::cbegin(unfinalizedIdentifierStr))
                ) {
                break;
            }

            // Read out data.
            creationTool = std::string(reinterpret_cast<char const*>(data->toolStr), sizeof(data->toolStr));
            versionNum = data->versionNum.value();
            versionStr = std::string(reinterpret_cast<char const*>(data->versionStr), sizeof(data->versionStr));

            // Read any finalization flags.
            finalizationFlags = FinalizationFlags(data->finalizationFlags.value());
            customFinalizationFlags = data->customFinalizationFlags.value();

            result = true;
        } while(false);

        return result;
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