#ifndef TOOLS_PCAP_CAN_EXPORTER_H
#define TOOLS_PCAP_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include <ctime>
#include <sstream>

#include "CAN_DataFrame.h"
#include "Library.h"

namespace tools::pcap {

class PCAP_CAN_Exporter : public tools::shared::GenericRecordExporter<CAN_DataFrame_t> {
    public:
        explicit PCAP_CAN_Exporter(std::ostream& output, FileInfo_t const& fileInfo);
        void writeHeader() override;
        void writeRecord(CAN_DataFrame_t record) override;
    private:
        FileInfo_t const& fileInfo;
    };

}

#endif //TOOLS_PCAP_CAN_EXPORTER_H
