#ifndef TOOLS_PCAP_CAN_EXPORTER_H
#define TOOLS_PCAP_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CANRecord.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::pcap {

  class PCAP_CAN_Exporter : public mdf::tools::shared::GenericRecordExporter<mdf::CANRecord> {
  public:
    explicit PCAP_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const& fileInfo);

    /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeHeader */
    void writeHeader() override;

    /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeRecord */
    void writeRecord(mdf::CANRecord const& record) override;

  private:
    mdf::tools::shared::ParsedFileInfo const& fileInfo;
  };

}

#endif //TOOLS_PCAP_CAN_EXPORTER_H
