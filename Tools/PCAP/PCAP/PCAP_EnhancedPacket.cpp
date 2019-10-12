#include "PCAP_EnhancedPacket.h"

namespace tools::pcap {

    uint16_t const epb_flags = 2;

    PCAP_EnhancedPacket::PCAP_EnhancedPacket() : PCAP_Block(0x00000006) {
        optionFlags = std::make_shared<PCAP_OptionU32>(epb_flags);

        interfaceIndex = 0;
        timeStampHigh = 0;
        timeStampLow = 0;
        capturedPacketLength = 0;
        originalPacketLength = 0;

        options.addOption(optionFlags);

        updateBlockLength();
    }

    void PCAP_EnhancedPacket::serialize(std::ostream &stream) const {
        stream.write(reinterpret_cast<char const*>(&interfaceIndex), sizeof(interfaceIndex));
        stream.write(reinterpret_cast<char const*>(&timeStampHigh), sizeof(timeStampHigh));
        stream.write(reinterpret_cast<char const*>(&timeStampLow), sizeof(timeStampLow));
        stream.write(reinterpret_cast<char const*>(&capturedPacketLength), sizeof(capturedPacketLength));
        stream.write(reinterpret_cast<char const*>(&originalPacketLength), sizeof(originalPacketLength));

        serializePacket(stream);

        stream << options;
    }

    void PCAP_EnhancedPacket::updateBlockLength() {
        blockLength = 12;
        blockLength += sizeof(interfaceIndex);
        blockLength += sizeof(timeStampHigh);
        blockLength += sizeof(timeStampLow);
        blockLength += sizeof(capturedPacketLength);
        blockLength += sizeof(originalPacketLength);
        blockLength += options.getSize();
    }

}
