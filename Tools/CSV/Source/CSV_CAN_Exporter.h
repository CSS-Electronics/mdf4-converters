#ifndef TOOLS_CSV_CAN_EXPORTER_H
#define TOOLS_CSV_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CSV_Common.h"

namespace mdf::tools::csv {

    class CSV_CAN_Exporter : public mdf::tools::shared::GenericRecordExporter<CSV_Record> {
    public:
        explicit CSV_CAN_Exporter(std::ostream &output, std::string delimiter = ";");

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeHeader */
        void writeHeader() override;

        /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeRecord */
        void writeRecord(CSV_Record const &record) override;

    private:
        /*! String representing the delimiter used between value field. */
        std::string const delimiter;

        void write_CAN_DataFrame(mdf::CAN_DataFrame const &record);

        void write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record);
    };

}

#endif //TOOLS_CSV_CAN_EXPORTER_H
