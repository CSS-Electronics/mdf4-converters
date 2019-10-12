#include "PCAP_SectionHeader.h"

#include "PCAP_OptionString.h"

namespace tools::pcap {

    uint16_t const shb_hardware = 2;
    uint16_t const shb_os = 3;
    uint16_t const shb_userappl = 4;

    PCAP_SectionHeader::PCAP_SectionHeader() :
        PCAP_Block(0x0A0D0D0A),
        byteOrderMagic(0x1A2B3C4D),
        majorVersion(1),
        minorVersion(0),
        sectionLink(-1) {

        // Add options.
        options.addOption(PCAP_OptionString(shb_userappl, "mdf2pcap"));

        // Update block length.
        blockLength = 12 + sizeof(byteOrderMagic) + sizeof(majorVersion) + sizeof(minorVersion) + sizeof(sectionLink) + options.getSize();
    }

    void PCAP_SectionHeader::serialize(std::ostream &stream) const {
        // Write data.
        stream.write(reinterpret_cast<char const*>(&byteOrderMagic), sizeof(byteOrderMagic));
        stream.write(reinterpret_cast<char const*>(&majorVersion), sizeof(majorVersion));
        stream.write(reinterpret_cast<char const*>(&minorVersion), sizeof(minorVersion));
        stream.write(reinterpret_cast<char const*>(&sectionLink), sizeof(sectionLink));

        // Write the options.
        stream << options;
    }

}
