#ifndef TOOLS_CLX000_LIN_EXPORTER_H
#define TOOLS_CLX000_LIN_EXPORTER_H

#include "LIN_Frame.h"
#include "CLX000_BaseExporter.h"

namespace mdf::tools::clx {

    struct CLX000_LIN_Exporter : public CLX000_BaseExporter<LIN_Frame> {
    public:
        explicit CLX000_LIN_Exporter(
            std::ostream& output,
            mdf::tools::shared::ParsedFileInfo const& fileInfo,
            uint8_t busChannel,
            CLX000_Configuration const& configuration,
            tools::shared::DisplayTimeFormat displayLocalTime
        );

        void writeRecord(LIN_Frame const& record) override;
    };

}

#endif //TOOLS_CLX000_LIN_EXPORTER_H
