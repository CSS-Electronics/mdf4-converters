#ifndef MDFSIMPLECONVERTERS_SIBLOCK_H
#define MDFSIMPLECONVERTERS_SIBLOCK_H

#include "MdfBlock.h"
#include "MDBlock.h"

namespace mdf {

    enum struct SIBlockBusType : uint8_t {
        CAN = 0x02u,
        LIN = 0x03u
    };

    SIBlockBusType operator&(SIBlockBusType const& lhs, SIBlockBusType const& rhs);

    struct SIBlock : MdfBlock {
        [[nodiscard]] SIBlockBusType getBusType() const;
        [[nodiscard]] std::shared_ptr<MDBlock> getComment() const;
    protected:
        bool load(std::shared_ptr<std::streambuf> stream) override;
        bool saveBlockData(uint8_t * dataPtr) override;
    private:
        uint8_t busType;
        uint8_t flags;
        uint8_t type;
    };

}

#endif //MDFSIMPLECONVERTERS_SIBLOCK_H
