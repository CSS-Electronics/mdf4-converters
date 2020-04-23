#ifndef TOOLS_SOCKETCAN_LIN_EXPORTER_H
#define TOOLS_SOCKETCAN_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LINRecord.h"
#include "CommonOptions.h"
#include "FileInfo.h"

namespace mdf::tools::socketcan {

  class SocketCAN_LIN_Exporter : public tools::shared::GenericRecordExporter<LINRecord> {
  public:
    explicit SocketCAN_LIN_Exporter(std::ostream &output, FileInfo const &fileInfo,
                                    tools::shared::DisplayTimeFormat displayLocalTime);

    void writeHeader() override;

    void writeRecord(LINRecord const& record) override;

  private:
    tools::shared::DisplayTimeFormat displayLocalTime;
    FileInfo const &fileInfo;
  };

}
#endif //TOOLS_SOCKETCAN_LIN_EXPORTER_H
