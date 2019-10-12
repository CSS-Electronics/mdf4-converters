#include "PCAP_Option.h"

namespace tools::pcap {

    PCAP_Option::PCAP_Option(uint16_t optionCode) : optionCode(optionCode), optionLength(0) {

    }

    PCAP_Option::~PCAP_Option() = default;

    uint32_t PCAP_Option::getSize() const {
        uint32_t result = sizeof(optionCode) + sizeof(optionLength);

        return result;
    }

    uint16_t PCAP_Option::getOptionCode() const {
        return optionCode;
    }

    std::ostream& operator<<(std::ostream& stream, PCAP_Option const& data) {
        uint16_t optionLength = data.getSize() - 4;

        // Write the header data.
        stream.write(reinterpret_cast<char const*>(&data.optionCode), sizeof(data.optionCode));
        stream.write(reinterpret_cast<char const*>(&optionLength), sizeof(optionLength));

        // Let the option handle private serialization.
        data.serialize(stream);

        // Ensure the boundary is 32 bit aligned.
        auto currentPosition = stream.tellp();

        if(currentPosition % 4 != 0) {
            char paddingData[4] = { 0 };
            unsigned int padding = 4 - currentPosition % 4;
            stream.write(paddingData, padding);
        }

        return stream;
    }

}
