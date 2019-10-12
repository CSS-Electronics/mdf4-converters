#include "PCAP_CAN_Exporter.h"

#include "PCAP/PCAP_InterfaceDescription.h"
#include "PCAP/PCAP_SectionHeader.h"
#include "PCAP/PCAP_EnhancedPacket_CAN.h"

namespace tools::pcap {

    PCAP_CAN_Exporter::PCAP_CAN_Exporter(std::ostream& output, FileInfo_t const& fileInfo) : GenericRecordExporter(output), fileInfo(fileInfo) {

    }

    void PCAP_CAN_Exporter::writeHeader() {
        // Write the main header.
        PCAP_SectionHeader sectionHeader;

        output << sectionHeader;

        // Write interface descriptors.
        PCAP_InterfaceDescription can1;
        PCAP_InterfaceDescription can2;
        PCAP_InterfaceDescription lin1;
        PCAP_InterfaceDescription lin2;

        can1.setDescription("CAN Bus 1");
        can1.setName("CAN 1");
        can1.setSpeed(fileInfo.BitrateCAN1);
        can1.linkType = 113;

        can2.setDescription("CAN Bus 2");
        can2.setName("CAN 2");
        can2.setSpeed(fileInfo.BitrateCAN2);
        can2.linkType = 113;

        lin1.setDescription("LIN Bus 1");
        lin1.setName("LIN 1");
        lin1.setSpeed(fileInfo.BitrateLIN1);
        lin1.linkType = 212;

        lin2.setDescription("LIN Bus 2");
        lin2.setName("LIN 2");
        lin2.setSpeed(fileInfo.BitrateLIN2);
        lin2.linkType = 212;

        output << can1;
        output << can2;
        output << lin1;
        output << lin2;
    }

    void PCAP_CAN_Exporter::writeRecord(CAN_DataFrame_t record) {
        output << PCAP_EnhancedPacket_CAN(record);
    }

}
