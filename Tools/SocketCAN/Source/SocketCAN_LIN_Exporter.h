#ifndef TOOLS_SOCKETCAN_LIN_EXPORTER_H
#define TOOLS_SOCKETCAN_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LINRecord.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::socketcan {

  class SocketCAN_LIN_Exporter : public tools::shared::GenericRecordExporter<LINRecord> {
  public:
    explicit SocketCAN_LIN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                    tools::shared::DisplayTimeFormat displayLocalTime);

    void writeHeader() override;

    void writeRecord(LINRecord const& record) override;

  private:
    tools::shared::DisplayTimeFormat displayLocalTime;
    mdf::tools::shared::ParsedFileInfo const &fileInfo;
  };

}
#endif //TOOLS_SOCKETCAN_LIN_EXPORTER_H
