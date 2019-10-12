#ifndef TOOLS_PCAP_PCAP_ENHANCEDPACKET_H
#define TOOLS_PCAP_PCAP_ENHANCEDPACKET_H

#include <memory>

#include "PCAP_Block.h"
#include "PCAP_OptionList.h"
#include "PCAP_OptionU32.h"

namespace tools::pcap {

    class PCAP_EnhancedPacket : public PCAP_Block {
    public:
        PCAP_EnhancedPacket();

        uint32_t interfaceIndex;
        uint32_t timeStampHigh;
        uint32_t timeStampLow;
        uint32_t capturedPacketLength;
        uint32_t originalPacketLength;
    protected:
        void updateBlockLength();
        virtual void serializePacket(std::ostream &stream) const = 0;
    private:
        std::shared_ptr<PCAP_OptionU32> optionFlags;
        PCAP_OptionList options;

        void serialize(std::ostream &stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_ENHANCEDPACKET_H
