#include "PCAP_OptionU32.h"

namespace mdf::tools::pcap {

    PCAP_OptionU32::PCAP_OptionU32(uint16_t optionCode) : PCAP_Option(optionCode) {
        data = 0;
    }

    PCAP_OptionU32::PCAP_OptionU32(uint16_t optionCode, uint32_t data) : PCAP_OptionU32(optionCode) {
        this->data = data;
    }

    PCAP_OptionU32* PCAP_OptionU32::clone() const {
        return new PCAP_OptionU32(getOptionCode(), data);
    }

    void PCAP_OptionU32::serialize(std::ostream &stream) const {
        stream.write(reinterpret_cast<char const*>(&data), sizeof(data));
    }

    uint32_t PCAP_OptionU32::getSize() const {
        return 4 + sizeof(data);
    }

}
