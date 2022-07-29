#include <fstream>

#include "PCAP_Exporter.h"

#include "MdfFile.h"
#include "PCAP_CAN_Exporter.h"
#include "ProgressIndicator.h"
#include "ParsedFileInfo.h"
#include "Version.h"

using namespace mdf;

namespace mdf::tools::pcap {

    PCAP_Exporter::PCAP_Exporter() : ConverterInterface(PROGRAM_NAME) {

    }

    bool PCAP_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
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
                auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.pcap");
                std::ofstream output(outputFilePathCAN.string(), std::ios::binary | std::ios::out);

                // Create a CAN exporter.
                PCAP_CAN_Exporter exporter(output, info);

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

            status = true;
        }

        return status;
    }

    semver::version const& PCAP_Exporter::getVersion() const {
        return Version;
    }

}
