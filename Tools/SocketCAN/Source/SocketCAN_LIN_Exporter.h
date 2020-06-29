#ifndef TOOLS_SOCKETCAN_LIN_EXPORTER_H
#define TOOLS_SOCKETCAN_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LIN_Frame.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::socketcan {

    class SocketCAN_LIN_Exporter : public tools::shared::GenericRecordExporter<LIN_Frame> {
    public:
        explicit SocketCAN_LIN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                        tools::shared::DisplayTimeFormat displayLocalTime);

        void writeHeader() override;

        void writeRecord(LIN_Frame const &record) override;

    private:
        tools::shared::DisplayTimeFormat displayLocalTime;
        mdf::tools::shared::ParsedFileInfo const &fileInfo;
    };

}
#endif //TOOLS_SOCKETCAN_LIN_EXPORTER_H
