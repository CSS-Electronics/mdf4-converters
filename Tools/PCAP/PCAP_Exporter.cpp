#include "PCAP_Exporter.h"

#include "Library.h"
#include "PCAP_CAN_Exporter.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace tools::pcap {

    PCAP_Exporter::PCAP_Exporter() : ConverterInterface("mdf2pcap") {

    }

    bool PCAP_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
        bool status = false;

        // Attempt to open the input file.
        MdfFile mdfFile(inputFilePath.string());

        mdfFile.finalize();
        mdfFile.sort();

        // Create the base file name.
        std::string const outputFileBase = inputFilePath.stem().string() + "_";

        // Determine if any data is present.
        FileInfo_t info = mdfFile.getFileInfo();

        // Determine the number of data records.
        auto const totalRecords = static_cast<unsigned int>(info.CANMessages);
        auto const singleStep = int(totalRecords / 100.0);

        if(info.CANMessages > 0) {
            // Create an output for the CAN messages.
            auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.pcap");
            std::ofstream output(outputFilePathCAN.string(), std::ios::binary | std::ios::out);

            // Create a CAN exporter.
            PCAP_CAN_Exporter exporter(output, info);

            // Write the header.
            exporter.writeHeader();

            // Start the progress by signaling zero progress.
            updateProgress(0, info.CANMessages);

            // Loop over the data and write each record.
            auto iterPtr = mdfFile.getCANDataFrameIterator();
            auto iterEndPtr = mdfFile.getCANDataFrameIteratorEnd();

            auto iter = *iterPtr;
            auto iterEnd = *iterEndPtr;
            int counter = 0;

            while (iter != iterEnd) {
                std::shared_ptr<CAN_DataFrame_t> data = *iter;
                exporter.writeRecord(*data.get());
                ++iter;
                ++counter;

                // Only update for each step, instead of for each record.
                if((counter % singleStep) == 0) {
                    updateProgress(counter, info.CANMessages);
                }
            }

            // Complete the progress by signaling zero progress.
            updateProgress(info.CANMessages, info.CANMessages);
        }

        status = true;

        return status;
    }

    interfaces::Version PCAP_Exporter::getVersion() const {
        return tools::pcap::version;
    }

}
