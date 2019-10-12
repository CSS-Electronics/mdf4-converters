#include "PCAP_OptionString.h"

namespace tools::pcap {

    PCAP_OptionString::PCAP_OptionString(uint16_t optionCode) : PCAP_Option(optionCode) {

    }

    PCAP_OptionString::PCAP_OptionString(uint16_t optionCode, std::string data) : PCAP_OptionString(optionCode) {
        this->data = std::move(data);
    }

    PCAP_OptionString* PCAP_OptionString::clone() const {
        return new PCAP_OptionString(getOptionCode(), data);
    }

    void PCAP_OptionString::serialize(std::ostream &stream) const {
        stream << data;
    }

    uint32_t PCAP_OptionString::getSize() const {
        return 4 + data.size();
    }

}
