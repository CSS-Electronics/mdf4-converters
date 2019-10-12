#include "CSV_Exporter.h"

#include "CSV_CAN_Exporter.h"
#include "CSV_LIN_Exporter.h"
#include "Library.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace tools::csv {

    CSV_Exporter::CSV_Exporter() : ConverterInterface("mdf2csv") {

    }

    bool CSV_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
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
        auto const totalRecords = static_cast<unsigned int>(info.CANMessages + info.LINMessages);
        auto const singleStep = int(totalRecords / 100.0);

        if(info.CANMessages > 0) {
            // Create an output for the CAN messages.
            auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.csv");
            std::ofstream output(outputFilePathCAN.string());

            // Create a CAN exporter.
            CSV_CAN_Exporter exporter(output);

            // Write the header.
            exporter.writeHeader();

            // Start the progress by signaling zero progress.
            updateProgress(0, totalRecords);

            // Loop over the data and write each record.
            auto iterPtr = mdfFile.getCANDataFrameIterator();
            auto iterEndPtr = mdfFile.getCANDataFrameIteratorEnd();

            auto iter = *iterPtr;
            auto iterEnd = *iterEndPtr;
            unsigned int counter = 0;

            while (iter != iterEnd) {
                std::shared_ptr<CAN_DataFrame_t> data = *iter;
                exporter.writeRecord(*data.get());
                ++iter;
                ++counter;

                // Only update for each step, instead of for each record.
                if((counter % singleStep) == 0) {
                    updateProgress(counter, totalRecords);
                }
            }

            // Complete the progress by signaling zero progress.
            updateProgress(info.CANMessages, totalRecords);
        }

        if(info.LINMessages > 0) {
            // Create an output for the CAN messages.
            auto outputFilePathLIN = outputFolder / (outputFileBase + "LIN.csv");
            std::ofstream output(outputFilePathLIN.string());

            // Create a CAN exporter.
            CSV_LIN_Exporter exporter(output);

            // Write the header.
            exporter.writeHeader();

            // Start the progress by signaling zero progress.
            updateProgress(info.CANMessages, totalRecords);

            // Loop over the data and write each record.
            auto iterPtr = mdfFile.getLINFrameIterator();
            auto iterEndPtr = mdfFile.getLINFrameIteratorEnd();

            auto iter = *iterPtr;
            auto iterEnd = *iterEndPtr;
            unsigned int counter = info.CANMessages;

            while (iter != iterEnd) {
                std::shared_ptr<LIN_Frame_t> data = *iter;
                exporter.writeRecord(*data.get());
                ++iter;
                ++counter;

                // Only update for each step, instead of for each record.
                if((counter % singleStep) == 0) {
                    updateProgress(counter, totalRecords);
                }
            }

            // Complete the progress by signaling zero progress.
            updateProgress(totalRecords, totalRecords);
        }

        status = true;

        return status;
    }
    
    interfaces::Version CSV_Exporter::getVersion() const {
        return tools::csv::version;
    }

}
