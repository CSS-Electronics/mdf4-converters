#ifndef TOOLS_PEAK_EXPORTER_H
#define TOOLS_PEAK_EXPORTER_H

#include "ConverterInterface.h"
#include "PEAK_TraceFormat.h"

namespace mdf::tools::peak {

  class PEAK_Exporter : public tools::shared::ConverterInterface {
  public:
    PEAK_Exporter();

    void configureParser(boost::program_options::options_description &opts) override;

    bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;

    [[nodiscard]] Version getVersion() const override;

  private:
    PEAK_TraceFormat traceFormat;
  };

}

#endif //TOOLS_PCAP_EXPORTER_H
