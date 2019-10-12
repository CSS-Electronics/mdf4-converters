#ifndef TOOLS_SOCKETCAN_LIN_EXPORTER_H
#define TOOLS_SOCKETCAN_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LIN_Frame.h"
#include "CommonOptions.h"
#include "Library.h"

namespace tools::socketcan {

    class SocketCAN_LIN_Exporter : public tools::shared::GenericRecordExporter<LIN_Frame_t> {
    public:
        explicit SocketCAN_LIN_Exporter(std::ostream& output, FileInfo_t const& fileInfo, tools::shared::DisplayTimeFormat displayLocalTime);
        void writeHeader() override;
        void writeRecord(LIN_Frame_t record) override;
    private:
        tools::shared::DisplayTimeFormat displayLocalTime;
        FileInfo_t const& fileInfo;
    };

}
#endif //TOOLS_SOCKETCAN_LIN_EXPORTER_H
