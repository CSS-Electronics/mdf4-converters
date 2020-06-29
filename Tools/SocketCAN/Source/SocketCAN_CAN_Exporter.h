#ifndef TOOLS_SOCKETCAN_CAN_EXPORTER_H
#define TOOLS_SOCKETCAN_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CAN_DataFrame.h"
#include "CommonOptions.h"
#include "ParsedFileInfo.h"

namespace mdf::tools::socketcan {

    class SocketCAN_CAN_Exporter : public tools::shared::GenericRecordExporter<CAN_DataFrame> {
    public:
        explicit SocketCAN_CAN_Exporter(std::ostream &output, mdf::tools::shared::ParsedFileInfo const &fileInfo,
                                        tools::shared::DisplayTimeFormat displayLocalTime);

        void writeHeader() override;

        void writeRecord(CAN_DataFrame const &record) override;

    private:
        tools::shared::DisplayTimeFormat displayLocalTime;
        mdf::tools::shared::ParsedFileInfo const &fileInfo;
    };

}
#endif //TOOLS_SOCKETCAN_CAN_EXPORTER_H
