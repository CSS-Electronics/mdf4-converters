#ifndef TOOLS_PCAP_PCAP_SECTIONHEADER_H
#define TOOLS_PCAP_PCAP_SECTIONHEADER_H

#include "PCAP_Block.h"
#include "PCAP_OptionList.h"

namespace mdf::tools::pcap {

    class PCAP_SectionHeader : public PCAP_Block {
    public:
        PCAP_SectionHeader();
    private:
        uint32_t const byteOrderMagic;
        uint16_t const majorVersion;
        uint16_t const minorVersion;
        PCAP_OptionList options;
        int64_t sectionLink;

        void serialize(std::ostream& stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_SECTIONHEADER_H
