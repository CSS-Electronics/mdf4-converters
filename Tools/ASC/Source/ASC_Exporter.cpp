#include "ASC_Exporter.h"

#include "ASC_CAN_Exporter.h"
#include "ASC_LIN_Exporter.h"
#include "MdfFile.h"
#include "ProgressIndicator.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace mdf::tools::asc {

  ASC_Exporter::ASC_Exporter() : ConverterInterface("mdf2asc") {

  }

  bool ASC_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
    bool status = false;

    // Attempt to open the input file.
    std::unique_ptr<MdfFile> mdfFile = MdfFile::Create(inputFilePath.string());

    if (mdfFile) {
      // Create the base file name.
      std::string const outputFileBase = inputFilePath.stem().string() + "_";

      // Load full metadata.
      FileInfo fileInfo = mdfFile->getFileInfo();
      MetadataMap metadata = mdfFile->getMetadata();
      mdf::tools::shared::ParsedFileInfo info(fileInfo, metadata);

      if (info.CANMessages > 0) {
        // Create an output for the CAN messages.
        auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.asc");
        std::ofstream output(outputFilePathCAN.string());

        // Create a CAN exporter.
        ASC_CAN_Exporter exporter(output, info, commonOptions->displayTimeFormat);

        // Write the header.
        exporter.writeHeader();

        mdf::tools::shared::ProgressIndicator indicator(0, info.CANMessages, commonOptions->nonInteractiveMode);
        indicator.setPrefix("CAN");
        std::size_t i = 0;

        // Extract iterator and write all records.
        auto iter = mdfFile->getCANIterator();

        indicator.begin();
        for (auto const &entry: iter) {
          exporter.writeRecord(entry);
          indicator.update(++i);
        }
        indicator.end();
      }

      if (info.LINMessages > 0) {
        // Create an output for the CAN messages.
        auto outputFilePathLIN = outputFolder / (outputFileBase + "LIN.asc");
        std::ofstream output(outputFilePathLIN.string());

        // Create a CAN exporter.
        ASC_LIN_Exporter exporter(output, info, commonOptions->displayTimeFormat);

        // Write the header.
        exporter.writeHeader();

        mdf::tools::shared::ProgressIndicator indicator(0, info.LINMessages, commonOptions->nonInteractiveMode);
        indicator.setPrefix("LIN");
        std::size_t i = 0;

        // Extract iterator and write all records.
        auto iter = mdfFile->getLINIterator();

        indicator.begin();
        for (auto const &entry: iter) {
          exporter.writeRecord(entry);
          indicator.update(++i);
        }
        indicator.end();
      }

      status = true;
    }

    return status;
  }

  Version ASC_Exporter::getVersion() const {
    return mdf::tools::asc::version;
  }

}
