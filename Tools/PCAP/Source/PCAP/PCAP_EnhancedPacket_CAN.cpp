#include "PCAP_EnhancedPacket_CAN.h"

#include <chrono>
#include <algorithm>

namespace mdf::tools::pcap {

    PCAP_EnhancedPacket_CAN::PCAP_EnhancedPacket_CAN(mdf::CANRecord const& canDataFrame) : data({0}) {
        // Set options for Linux SLL.
        link_layer_address_length = 0;
        link_layer_address = 0x00000000;
        ARPHRD_type = 0;

        // Determine the direction.
        if(canDataFrame.Dir) {
            // Transmitted by us.
            packet_type = 4;
        } else {
            // Received broadcast.
            packet_type = 1;
        }

        // Is this a FD frame?
        if(canDataFrame.EDL) {
            protocol_type = 0x000D;

            // Set the BRS bit if present.
            brs = canDataFrame.BRS;
        } else {
            protocol_type = 0x000C;
            brs = false;
        }

        // Copy over the id.
        id = canDataFrame.ID;

        // If extended IDs are used, set the flag as well.
        if(canDataFrame.IDE) {
            id |= 0x80000000;
        }

        // Convert the DLC to a length.
        length = canDataFrame.DataLength;
        std::copy_n(std::begin(canDataFrame.DataBytes), length, data.begin());

        // Update the length field of the PCAP packet.
        capturedPacketLength = 16 + 8 + length;
        originalPacketLength = capturedPacketLength;

        // Update the timestamps. Since no 'if_tsresol' has been set, the standard resolution of microseconds is
        // assumed.
        std::chrono::nanoseconds rawTimeStamp(canDataFrame.TimeStamp);
        std::chrono::microseconds timeStampMicros = std::chrono::duration_cast<std::chrono::microseconds>(rawTimeStamp);
        
        uint64_t timeStamp = timeStampMicros.count();

        timeStampHigh = (timeStamp & 0xFFFFFFFF00000000u) >> 32u;
        timeStampLow = (timeStamp & 0x00000000FFFFFFFFu);

        // Set the interface ID.
        if(canDataFrame.BusChannel == 1) {
            interfaceIndex = 0;
        } else {
            interfaceIndex = 1;
        }

        updateBlockLength();
        blockLength += capturedPacketLength;

        // Ensure the data is aligned with a 32 bit boundary.
        if(blockLength % 4 != 0) {
            paddingLength = 4 - blockLength % 4;
            blockLength += paddingLength;
        }
    }

    void PCAP_EnhancedPacket_CAN::serializePacket(std::ostream &stream) const {
        auto begin = stream.tellp();

        // Write the packet.
        char dataByte;

        // Start with the packet type MSB.
        dataByte = static_cast<char>(packet_type >> 8);
        stream << dataByte;
        dataByte = static_cast<char>(packet_type & 0xFF);
        stream << dataByte;

        dataByte = static_cast<char>(ARPHRD_type >> 8);
        stream << dataByte;
        dataByte = static_cast<char>(ARPHRD_type & 0xFF);
        stream << dataByte;

        dataByte = static_cast<char>(link_layer_address_length >> 8);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address_length & 0xFF);
        stream << dataByte;

        dataByte = static_cast<char>(link_layer_address >> 56);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 48);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 40);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 32);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 24);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 16);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address >> 8);
        stream << dataByte;
        dataByte = static_cast<char>(link_layer_address & 0xFF);
        stream << dataByte;

        dataByte = static_cast<char>(protocol_type >> 8);
        stream << dataByte;
        dataByte = static_cast<char>(protocol_type & 0xFF);
        stream << dataByte;

        // Write the data from this.
        stream.write(reinterpret_cast<char const*>(&id), sizeof(id));
        stream.write(reinterpret_cast<char const*>(&length), sizeof(length));

        char flags[3] = { 0x00 };

        // Handle FD field.
        if(brs) {
            flags[0] = 0x01;
        }

        stream.write(flags, 3);

        // Write the data.
        auto dataPtr = reinterpret_cast<char const*>(data.data());
        stream.write(dataPtr, length);
        auto end = stream.tellp();
        auto difference = end - begin;

        // Write the padding.
        char paddingData[4] = { 0 };
        stream.write(paddingData, paddingLength);
    }

}
