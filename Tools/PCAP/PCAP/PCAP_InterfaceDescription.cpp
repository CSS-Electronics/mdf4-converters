#include "PCAP_InterfaceDescription.h"

namespace tools::pcap {

    uint16_t const if_name = 2;
    uint16_t const if_description = 3;
    uint16_t const if_speed = 8;

    PCAP_InterfaceDescription::PCAP_InterfaceDescription() : PCAP_Block(0x00000001) {
        optionBitRate = std::make_shared<PCAP_OptionU64>(if_speed);
        optionDescription = std::make_shared<PCAP_OptionString>(if_description);
        optionName = std::make_shared<PCAP_OptionString>(if_name);

        linkType = 0;
        snapLen = 0;

        // Register options with list.
        options.addOption(optionBitRate);
        options.addOption(optionDescription);
        options.addOption(optionName);
    }

    void PCAP_InterfaceDescription::setDescription(std::string description) {
        optionDescription->data = std::move(description);
        updateSize();
    }

    void PCAP_InterfaceDescription::serialize(std::ostream& stream) const {
        uint16_t padding = 0;

        // Write data.
        stream.write(reinterpret_cast<char const*>(&linkType), sizeof(linkType));
        stream.write(reinterpret_cast<char const*>(&padding), sizeof(padding));
        stream.write(reinterpret_cast<char const*>(&snapLen), sizeof(snapLen));

        // Write the options.
        stream << options;
    }

    void PCAP_InterfaceDescription::setName(std::string name) {
        optionName->data = std::move(name);
        updateSize();
    }

    void PCAP_InterfaceDescription::setSpeed(uint64_t bitRate) {
        optionBitRate->data = bitRate;
        updateSize();
    }

    void PCAP_InterfaceDescription::updateSize() {
        blockLength = 12 + sizeof(linkType) + sizeof(snapLen) + sizeof(uint16_t) + options.getSize();
    }

}
