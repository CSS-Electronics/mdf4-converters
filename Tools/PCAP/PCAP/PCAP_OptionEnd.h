#ifndef TOOLS_PCAP_PCAP_OPTIONEND_H
#define TOOLS_PCAP_PCAP_OPTIONEND_H

#include "PCAP_Option.h"

namespace tools::pcap {

    class PCAP_OptionEnd : public PCAP_Option {
    public:
        PCAP_OptionEnd();
        [[nodiscard]] PCAP_OptionEnd* clone() const override;

    private:
        void serialize(std::ostream& stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_OPTIONEND_H
