#ifndef TOOLS_PCAP_CAN_EXPORTER_H
#define TOOLS_PCAP_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CAN_DataFrame.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::pcap {

    class PCAP_CAN_Exporter : public mdf::tools::shared::GenericRecordExporter<mdf::CAN_DataFrame> {
    public:
        explicit PCAP_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo);

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeHeader */
        void writeHeader() override;

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeRecord */
        void writeRecord(mdf::CAN_DataFrame const &record) override;

    private:
        mdf::tools::shared::ParsedFileInfo const &fileInfo;
    };

}

#endif //TOOLS_PCAP_CAN_EXPORTER_H
