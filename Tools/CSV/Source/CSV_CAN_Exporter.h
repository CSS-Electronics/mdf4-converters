#ifndef TOOLS_CSV_CAN_EXPORTER_H
#define TOOLS_CSV_CAN_EXPORTER_H

#include "GenericRecordExporter.h"

#include "CANRecord.h"

namespace mdf::tools::csv {

  class CSV_CAN_Exporter : public mdf::tools::shared::GenericRecordExporter<mdf::CANRecord> {
  public:
    explicit CSV_CAN_Exporter(std::ostream &output, std::string delimiter = ";");

    /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeHeader */
    void writeHeader() override;

    /*! @copydoc mdf::tools::shared::GenericRecordExporter::writeRecord */
    void writeRecord(mdf::CANRecord const& record) override;

  private:
    /*! String representing the delimiter used between value field. */
    std::string const delimiter;
  };

}

#endif //TOOLS_CSV_CAN_EXPORTER_H
