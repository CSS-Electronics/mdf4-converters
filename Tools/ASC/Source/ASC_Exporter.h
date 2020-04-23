#ifndef TOOLS_CSV_EXPORTER_H
#define TOOLS_CSV_EXPORTER_H

#include "ConverterInterface.h"

namespace mdf::tools::asc {

  class ASC_Exporter : public tools::shared::ConverterInterface {
  public:
    ASC_Exporter();

    bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;

    [[nodiscard]] Version getVersion() const override;

  private:

  };

}

#endif //TOOLS_CSV_EXPORTER_H
