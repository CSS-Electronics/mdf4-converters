#ifndef TOOLS_PCAP_PCAP_OPTIONU32_H
#define TOOLS_PCAP_PCAP_OPTIONU32_H

#include "PCAP_Option.h"

namespace mdf::tools::pcap {

    class PCAP_OptionU32 : public PCAP_Option {
    public:
        explicit PCAP_OptionU32(uint16_t optionCode);
        explicit PCAP_OptionU32(uint16_t optionCode, uint32_t data);

        [[nodiscard]] PCAP_OptionU32* clone() const override;
        [[nodiscard]] uint32_t getSize() const override;

        uint32_t data;
    private:
        void serialize(std::ostream& stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_OPTIONU32_H
