#include "PCAP_OptionEnd.h"

namespace tools::pcap {

    PCAP_OptionEnd::PCAP_OptionEnd() : PCAP_Option(0) {

    }

    PCAP_OptionEnd* PCAP_OptionEnd::clone() const {
        return new PCAP_OptionEnd();
    }

    void PCAP_OptionEnd::serialize(std::ostream &stream) const {
        // Do nothing.
    }

}
