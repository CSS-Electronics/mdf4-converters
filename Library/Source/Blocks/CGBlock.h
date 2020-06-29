#ifndef MDFSIMPLECONVERTERS_CGBLOCK_H
#define MDFSIMPLECONVERTERS_CGBLOCK_H

#include "MdfBlock.h"
#include "CNBlock.h"
#include "SIBlock.h"

#include <string>
#include <vector>

namespace mdf {

    enum struct CGBlockFlags: uint16_t {
        BUSEvents = (0x01u << 1u)
    };

    CGBlockFlags operator&(CGBlockFlags const& lhs, CGBlockFlags const& rhs);

    struct CGBlock : MdfBlock {
        [[nodiscard]] std::shared_ptr<CGBlock> getNextCGBlock() const;
        [[nodiscard]] std::shared_ptr<CNBlock> getFirstCNBlock() const;
        [[nodiscard]] std::shared_ptr<SIBlock> getSIBlock() const;

        [[nodiscard]] uint64_t getRecordID() const;
        [[nodiscard]] int64_t getRecordSize() const;
        [[nodiscard]] uint64_t getCycleCount() const;
        [[nodiscard]] CGBlockFlags getFlags() const;

        [[nodiscard]] bool isVLSD() const;

        void setNextCGBlock(std::shared_ptr<CGBlock>);

        void setCycleCount(uint64_t cycleCount);
        void setVLSDDataBytes(uint64_t dataBytes);
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(uint8_t * dataPtr) override;
    private:
        uint64_t cycleCount;
        uint32_t dataBytes;
        uint32_t invalDataBytes;
        uint16_t flags;
        uint64_t recordID;
        int64_t recordSize;

        char16_t pathSeparator;

        std::shared_ptr<CGBlock> nextCGBlock;
    };

}

#endif //MDFSIMPLECONVERTERS_CGBLOCK_H
