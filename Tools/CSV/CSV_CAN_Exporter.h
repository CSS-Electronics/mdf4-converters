#ifndef TOOLS_CSV_CAN_EXPORTER_H
#define TOOLS_CSV_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CAN_DataFrame.h"

namespace tools::csv {

class CSV_CAN_Exporter : public tools::shared::GenericRecordExporter<CAN_DataFrame_t> {
    public:
        explicit CSV_CAN_Exporter(std::ostream& output, std::string delimiter = ";");
        void writeHeader() override;
        void writeRecord(CAN_DataFrame_t record) override;
    private:
        std::string const delimiter;
    };

}

#endif //TOOLS_CSV_CAN_EXPORTER_H
