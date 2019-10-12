#ifndef TOOLS_CSV_LIN_EXPORTER_H
#define TOOLS_CSV_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LIN_Frame.h"

namespace tools::csv {

    class CSV_LIN_Exporter : public tools::shared::GenericRecordExporter<LIN_Frame_t> {
    public:
        explicit CSV_LIN_Exporter(std::ostream& output, std::string delimiter = ";");
        void writeHeader() override;
        void writeRecord(LIN_Frame_t record) override;
    private:
        std::string const delimiter;
    };

}

#endif //TOOLS_CSV_LIN_EXPORTER_H
