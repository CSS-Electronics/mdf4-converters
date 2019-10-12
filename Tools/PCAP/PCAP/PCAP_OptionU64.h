#ifndef TOOLS_PCAP_PCAP_OPTIONU64_H
#define TOOLS_PCAP_PCAP_OPTIONU64_H

#include "PCAP_Option.h"

namespace tools::pcap {

    class PCAP_OptionU64 : public PCAP_Option {
    public:
        explicit PCAP_OptionU64(uint16_t optionCode);
        explicit PCAP_OptionU64(uint16_t optionCode, uint64_t data);

        [[nodiscard]] PCAP_OptionU64* clone() const override;
        [[nodiscard]] uint32_t getSize() const override;

        uint64_t data;
    private:
        void serialize(std::ostream& stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_OPTIONU64_H
