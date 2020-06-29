#ifndef TOOLS_CLX_CAN_EXPORTER_H
#define TOOLS_CLX_CAN_EXPORTER_H

#include "CAN_DataFrame.h"
#include "CLX000_BaseExporter.h"

namespace mdf::tools::clx {

    struct CLX000_CAN_Exporter : public CLX000_BaseExporter<CAN_DataFrame> {
    public:
        explicit CLX000_CAN_Exporter(
            std::ostream &output,
            mdf::tools::shared::ParsedFileInfo const &fileInfo,
            uint8_t busChannel,
            CLX000_Configuration const &configuration,
            shared::DisplayTimeFormat displayLocalTime
        );

        void writeRecord(CAN_DataFrame const &record) override;
    };

}

#endif //TOOLS_CLX_CAN_EXPORTER_H
