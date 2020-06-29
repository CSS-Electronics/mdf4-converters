#ifndef TOOLS_CSV_LIN_EXPORTER_H
#define TOOLS_CSV_LIN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "LIN_Frame.h"

namespace mdf::tools::csv {

    class CSV_LIN_Exporter : public tools::shared::GenericRecordExporter<LIN_Frame> {
    public:
        explicit CSV_LIN_Exporter(std::ostream &output, std::string delimiter = ";");

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeHeader */
        void writeHeader() override;

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeRecord */
        void writeRecord(LIN_Frame const &record) override;

    private:
        /*! String representing the delimiter used between value field. */
        std::string const delimiter;
    };

}

#endif //TOOLS_CSV_LIN_EXPORTER_H
