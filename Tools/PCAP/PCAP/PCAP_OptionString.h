#ifndef TOOLS_PCAP_PCAP_OPTIONSTRING_H
#define TOOLS_PCAP_PCAP_OPTIONSTRING_H

#include <string>

#include "PCAP_Option.h"

namespace tools::pcap {

    class PCAP_OptionString : public PCAP_Option {
    public:
        explicit PCAP_OptionString(uint16_t optionCode);
        explicit PCAP_OptionString(uint16_t optionCode, std::string data);

        [[nodiscard]] PCAP_OptionString* clone() const override;
        [[nodiscard]] uint32_t getSize() const override;

        std::string data;
    private:
        void serialize(std::ostream& stream) const override;
    };


}

#endif //TOOLS_PCAP_PCAP_OPTIONSTRING_H
