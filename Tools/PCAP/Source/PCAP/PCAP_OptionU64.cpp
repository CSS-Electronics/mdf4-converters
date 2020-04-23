#include "PCAP_OptionU64.h"

namespace mdf::tools::pcap {

    PCAP_OptionU64::PCAP_OptionU64(uint16_t optionCode) : PCAP_Option(optionCode) {
        data = 0;
    }

    PCAP_OptionU64::PCAP_OptionU64(uint16_t optionCode, uint64_t data) : PCAP_OptionU64(optionCode) {
        this->data = data;
    }

    PCAP_OptionU64* PCAP_OptionU64::clone() const {
        return new PCAP_OptionU64(getOptionCode(), data);
    }

    void PCAP_OptionU64::serialize(std::ostream &stream) const {
        stream.write(reinterpret_cast<char const*>(&data), sizeof(data));
    }

    uint32_t PCAP_OptionU64::getSize() const {
        return 4 + sizeof(data);
    }

}
