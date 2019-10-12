#ifndef TOOLS_PCAP_PCAP_ENHANCEDPACKET_CAN_H
#define TOOLS_PCAP_PCAP_ENHANCEDPACKET_CAN_H

#include <array>

#include "PCAP_EnhancedPacket.h"
#include "CAN_DataFrame.h"

namespace tools::pcap {

    class PCAP_EnhancedPacket_CAN : public PCAP_EnhancedPacket {
    public:
        explicit PCAP_EnhancedPacket_CAN(CAN_DataFrame_t const& data);
    private:
        bool brs;
        uint32_t id;
        uint8_t length;
        std::array<uint8_t, 64> data;

        uint16_t packet_type;
        uint16_t ARPHRD_type;
        uint16_t link_layer_address_length;
        uint64_t link_layer_address;
        uint16_t protocol_type;

        unsigned int paddingLength = 0;

        void serializePacket(std::ostream& stream) const override;
    };

}

#endif //TOOLS_PCAP_PCAP_ENHANCEDPACKET_CAN_H
