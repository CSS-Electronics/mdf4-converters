#include "CLX000_Exporter.h"

#include "CLX000_CAN_Exporter.h"
#include "CLX000_LIN_Exporter.h"
#include "Library.h"
#include "ProjectInformation.h"

using namespace mdf;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

namespace tools::clx {

    CLX000_Exporter::CLX000_Exporter() : ConverterInterface(std::string(PROGRAM_NAME)), logOptions("log"), dataFieldsOptions("dataFields") {
        logOptions.add_options()
            ("log.valueSeparator", bpo::value<int>()->default_value(32), "")
            ("log.timestampFormat", bpo::value<int>()->default_value(4), "")
            ("log.timestampTimeSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timestampTimeMsSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timestampDateSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timeTimeDateSeparator", bpo::value<int>()->default_value(84), "")
            ;

        dataFieldsOptions.add_options()
            ("dataFields.timestamp", bpo::bool_switch()->default_value(true), "")
            ("dataFields.type", bpo::bool_switch()->default_value(false), "")
            ("dataFields.id", bpo::bool_switch()->default_value(true), "")
            ("dataFields.dataLength", bpo::bool_switch()->default_value(false), "")
            ("dataFields.data", bpo::bool_switch()->default_value(true), "")
            ;

        configuration.ValueSeparator[1] = '\0';
        configuration.TimestampTimeSeparator[1] = '\0';
        configuration.TimestampTimeMsSeparator[1] = '\0';
        configuration.TimestampDateSeparator[1] = '\0';
        configuration.TimeTimeDateSeparator[1] = '\0';
    }

    bool CLX000_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
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
            // Start the progress by signaling zero progress.
            unsigned int counter = 0;
            updateProgress(counter, totalRecords * 2);

            // Create an output for the CAN messages for each channel.
            for(int i = 1; i <= 2; ++i) {
                std::stringstream ss;
                ss << outputFileBase << "CAN_" << i << ".txt";
                auto outputFilePathCAN = outputFolder / ss.str();
                std::ofstream output(outputFilePathCAN.string());
                unsigned int recordsInChannel = 0;

                // Create a CAN exporter.
                CLX000_CAN_Exporter exporter(output, info, i, configuration, commonOptions->displayTimeFormat);

                // Write the header.
                exporter.writeHeader();

                // Loop over the data and write each record.
                auto iterPtr = mdfFile.getCANDataFrameIterator();
                auto iterEndPtr = mdfFile.getCANDataFrameIteratorEnd();

                auto iter = *iterPtr;
                auto iterEnd = *iterEndPtr;

                while (iter != iterEnd) {
                    std::shared_ptr<CAN_DataFrame_t> data = *iter;

                    // Ensure the channel matches the current.
                    if(data->BusChannel == i) {
                        exporter.writeRecord(*data.get());
                        ++recordsInChannel;
                    }
                    ++iter;
                    ++counter;

                    // Only update for each step, instead of for each record.
                    if((counter % singleStep) == 0) {
                        updateProgress(counter, totalRecords * 2);
                    }
                }

                // If no records were present in this channel, delete the file again.
                if(recordsInChannel == 0) {
                    try {
                        bfs::remove(outputFilePathCAN);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                    }
                } else {
                    exporter.correctHeader();
                }

                // Complete the progress by signaling full progress.
                updateProgress(counter, totalRecords * 2);

                // Ensure blank line at end of file.
                output << "\n";
            }
        }

        if(info.LINMessages > 0) {
            // Start the progress by signaling zero progress.
            unsigned int counter = info.CANMessages * 2;
            updateProgress(info.CANMessages * 2, totalRecords * 2);

            // Create an output for the LIN messages for each channel.
            for(int i = 1; i <= 2; ++i) {
                std::stringstream ss;
                ss << outputFileBase << "LIN_" << i << ".txt";
                auto outputFilePathLIN = outputFolder / ss.str();
                std::ofstream output(outputFilePathLIN.string());
                unsigned int recordsInChannel = 0;

                // Create a CAN exporter.
                CLX000_LIN_Exporter exporter(output, info, i, configuration, commonOptions->displayTimeFormat);

                // Write the header.
                exporter.writeHeader();

                // Loop over the data and write each record.
                auto iterPtr = mdfFile.getLINFrameIterator();
                auto iterEndPtr = mdfFile.getLINFrameIteratorEnd();

                auto iter = *iterPtr;
                auto iterEnd = *iterEndPtr;

                while (iter != iterEnd) {
                    std::shared_ptr<LIN_Frame_t> data = *iter;

                    // Ensure the channel matches the current.
                    if(data->BusChannel == i) {
                        exporter.writeRecord(*data.get());
                        ++recordsInChannel;
                    }
                    ++iter;
                    ++counter;

                    // Only update for each step, instead of for each record.
                    if((counter % singleStep) == 0) {
                        updateProgress(counter, totalRecords * 2);
                    }
                }

                // If no records were present in this channel, delete the file again.
                if(recordsInChannel == 0) {
                    try {
                        bfs::remove(outputFilePathLIN);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                    }
                } else {
                    exporter.correctHeader();
                }

                // Complete the progress by signaling full progress.
                updateProgress(counter, totalRecords * 2);

                // Ensure blank line at end of file.
                output << "\n";
            }
        }

        status = true;

        return status;
    }

    void CLX000_Exporter::configureFileParser(boost::program_options::options_description &opts) {
        opts.add(logOptions);
        opts.add(dataFieldsOptions);
    }

    interfaces::Version CLX000_Exporter::getVersion() const {
        return version;
    }

    tools::shared::ParseOptionStatus CLX000_Exporter::parseOptions(boost::program_options::variables_map const &result) {
        // Read out exporter options.
        configuration.ValueSeparator[0] = static_cast<char>(result["log.valueSeparator"].as<int>());
        configuration.TimestampFormat = result["log.timestampFormat"].as<int>();
        configuration.TimestampTimeSeparator[0] = static_cast<char>(result["log.timestampTimeSeparator"].as<int>());
        configuration.TimestampTimeMsSeparator[0] = static_cast<char>(result["log.timestampTimeMsSeparator"].as<int>());
        configuration.TimestampDateSeparator[0] = static_cast<char>(result["log.timestampDateSeparator"].as<int>());
        configuration.TimeTimeDateSeparator[0] = static_cast<char>(result["log.timeTimeDateSeparator"].as<int>());

        configuration.DataFields_timestamp = result["dataFields.timestamp"].as<bool>();
        configuration.DataFields_type = result["dataFields.type"].as<bool>();
        configuration.DataFields_id = result["dataFields.id"].as<bool>();
        configuration.DataFields_dataLength = result["dataFields.dataLength"].as<bool>();
        configuration.DataFields_data = result["dataFields.data"].as<bool>();

        return tools::shared::ParseOptionStatus::NoError;
    }

}
