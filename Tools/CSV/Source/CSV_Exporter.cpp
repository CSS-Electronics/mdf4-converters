#include "CSV_Exporter.h"
#include "CSV_CAN_Exporter.h"
#include "CSV_LIN_Exporter.h"
#include "MdfFile.h"
#include "ProgressIndicator.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace mdf::tools::csv {

  CSV_Exporter::CSV_Exporter() : ConverterInterface("mdf2csv") {

  }

  bool CSV_Exporter::convert(
    boost::filesystem::path inputFilePath,
    boost::filesystem::path outputFolder) {
    bool status = false;

    // Attempt to open the input file.
    std::unique_ptr<MdfFile> mdfFile = MdfFile::Create(inputFilePath.string());

    if (mdfFile) {
      // Create the base file name.
      std::string const outputFileBase = inputFilePath.stem().string() + "_";

      // Determine if any data is present.
      FileInfo info = mdfFile->getFileInfo();

      if(info.CANMessages > 0) {
        // Create an output for the CAN messages.
        auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.csv");
        std::ofstream output(outputFilePathCAN.string());

        // Create a CAN exporter.
        CSV_CAN_Exporter exporter(output);

        // Write the header.
        exporter.writeHeader();

        // Extract iterator.
        auto iter = mdfFile->getCANIterator();

        mdf::tools::shared::ProgressIndicator indicator(0, info.CANMessages, commonOptions->nonInteractiveMode);
        indicator.setPrefix("CAN");
        std::size_t i = 0;

        // Write all records.
        indicator.begin();
        for (auto const &entry: iter) {
          exporter.writeRecord(entry);
          indicator.update(++i);
        }
        indicator.end();
      }

      if(info.LINMessages > 0) {
        // Create an output for the LIN messages.
        auto outputFilePathCAN = outputFolder / (outputFileBase + "LIN.csv");
        std::ofstream output(outputFilePathCAN.string());

        // Create a LIN exporter.
        CSV_LIN_Exporter exporter(output);

        // Write the header.
        exporter.writeHeader();

        // Extract iterator.
        auto iter = mdfFile->getLINIterator();

        mdf::tools::shared::ProgressIndicator indicator(0, info.LINMessages, commonOptions->nonInteractiveMode);
        indicator.setPrefix("LIN");
        std::size_t i = 0;

        // write all records.
        indicator.begin();
        for (auto const &entry: iter) {
          exporter.writeRecord(entry);
        }
        indicator.end();
      }

      status = true;
    }

    return status;
  }

  Version CSV_Exporter::getVersion() const {
      return mdf::tools::csv::version;
  }

}
