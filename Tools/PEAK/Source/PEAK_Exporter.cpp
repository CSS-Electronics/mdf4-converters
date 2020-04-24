#include "PEAK_Exporter.h"

#include "MdfFile.h"
#include "PEAK_CAN_Exporter.h"
#include "PEAK_CAN_Exporter_1_1.h"
#include "PEAK_CAN_Exporter_2_1.h"
#include "ProgressIndicator.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace mdf::tools::peak {

  PEAK_Exporter::PEAK_Exporter() : ConverterInterface("mdf2peak") {

  }

  bool PEAK_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
    bool status = false;

    // Attempt to open the input file.
    std::unique_ptr<MdfFile> mdfFile = MdfFile::Create(inputFilePath.string());

    if (mdfFile) {
      // Create the base file name.
      std::string const outputFileBase = inputFilePath.stem().string() + "_";

      // Determine if any data is present.
      FileInfo info = mdfFile->getFileInfo();

      if (info.CANMessages > 0) {
        // Create an output for the CAN messages.
        auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.trc");
        std::ofstream output(outputFilePathCAN.string());

        // Create a CAN exporter.
        std::unique_ptr<PEAK_CAN_Exporter> exporter;

        switch (traceFormat) {
          case PEAK_TraceFormat::VERSION_1_1:
            exporter = std::make_unique<PEAK_CAN_Exporter_1_1>(output, info);
            break;
          case PEAK_TraceFormat::VERSION_2_1:
            exporter = std::make_unique<PEAK_CAN_Exporter_2_1>(output, info);
            break;
          default:
            break;
        }

        if (exporter) {
          // Write the header.
          exporter->writeHeader();

          // Extract iterator and write all records.
          auto iter = mdfFile->getCANIterator();

          mdf::tools::shared::ProgressIndicator indicator(0, info.CANMessages, commonOptions->nonInteractiveMode);
          indicator.setPrefix("CAN");
          std::size_t i = 0;

          // Write all records.
          indicator.begin();
          for (auto const &entry: iter) {
            exporter->writeRecord(entry);
            indicator.update(++i);
          }

          exporter->correctHeader();
          indicator.end();
        }
      }

      status = true;
    }

    return status;
  }

  Version PEAK_Exporter::getVersion() const {
    return mdf::tools::peak::version;
  }

  void PEAK_Exporter::configureParser(boost::program_options::options_description &opts) {
    // Add option for selecting which version to target.
    opts.add_options()
      ("trace-format,f", boost::program_options::value<PEAK_TraceFormat>(&traceFormat)->default_value(PEAK_TraceFormat::VERSION_2_1),
      "select which version to target");
  }

}
