#ifndef TOOLS_SOCKETCAN_CAN_EXPORTER_H
#define TOOLS_SOCKETCAN_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CAN_DataFrame.h"
#include "CommonOptions.h"
#include "Library.h"

namespace tools::socketcan {

class SocketCAN_CAN_Exporter : public tools::shared::GenericRecordExporter<CAN_DataFrame_t> {
    public:
        explicit SocketCAN_CAN_Exporter(std::ostream& output, FileInfo_t const& fileInfo, tools::shared::DisplayTimeFormat displayLocalTime);
        void writeHeader() override;
        void writeRecord(CAN_DataFrame_t record) override;
    private:
        tools::shared::DisplayTimeFormat displayLocalTime;
        FileInfo_t const& fileInfo;
};

}
#endif //TOOLS_SOCKETCAN_CAN_EXPORTER_H
