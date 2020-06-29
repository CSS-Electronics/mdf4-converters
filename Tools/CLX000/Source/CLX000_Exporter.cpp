#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include "CLX000_CAN_Exporter.h"
#include "CLX000_Exporter.h"
#include "CLX000_LIN_Exporter.h"
#include "MdfFile.h"
#include "ProgressIndicator.h"
#include "ParsedFileInfo.h"
#include "Version.h"

using namespace mdf;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

namespace mdf::tools::clx {

    CLX000_Exporter::CLX000_Exporter() : ConverterInterface(PROGRAM_NAME, true), logOptions("log"),
                                         dataFieldsOptions("dataFields") {
        logOptions.add_options()
            ("log.valueSeparator", bpo::value<int>()->default_value(32), "")
            ("log.timestampFormat", bpo::value<int>()->default_value(4), "")
            ("log.timestampTimeSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timestampTimeMsSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timestampDateSeparator", bpo::value<int>()->default_value(0), "")
            ("log.timeTimeDateSeparator", bpo::value<int>()->default_value(84), "");

        dataFieldsOptions.add_options()
            ("dataFields.timestamp", bpo::bool_switch()->default_value(true), "")
            ("dataFields.type", bpo::bool_switch()->default_value(false), "")
            ("dataFields.id", bpo::bool_switch()->default_value(true), "")
            ("dataFields.dataLength", bpo::bool_switch()->default_value(false), "")
            ("dataFields.data", bpo::bool_switch()->default_value(true), "");

        configuration.ValueSeparator[1] = '\0';
        configuration.TimestampTimeSeparator[1] = '\0';
        configuration.TimestampTimeMsSeparator[1] = '\0';
        configuration.TimestampDateSeparator[1] = '\0';
        configuration.TimeTimeDateSeparator[1] = '\0';
    }

    bool CLX000_Exporter::convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) {
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
                std::size_t recordsInChannel_1 = 0;
                std::size_t recordsInChannel_2 = 0;

                // Create an output for the CAN messages for each channel.
                auto const outputFilePathCAN_1 = outputFolder / (outputFileBase + "CAN_1.txt");
                auto const outputFilePathCAN_2 = outputFolder / (outputFileBase + "CAN_2.txt");

                // Create exporters for each channel and perform common setup.
                std::ofstream output_1(outputFilePathCAN_1.string());
                std::ofstream output_2(outputFilePathCAN_2.string());
                CLX000_CAN_Exporter exporter_1(output_1, info, 1, configuration, commonOptions->displayTimeFormat);
                CLX000_CAN_Exporter exporter_2(output_2, info, 2, configuration, commonOptions->displayTimeFormat);

                exporter_1.writeHeader();
                exporter_2.writeHeader();

                // Loop over all data and write to the correct log file.
                auto iter = mdfFile->getCANIterator();

                mdf::tools::shared::ProgressIndicator indicator(0, info.CANMessages, commonOptions->nonInteractiveMode);
                indicator.setPrefix("CAN");
                std::size_t i = 0;

                indicator.begin();
                for (auto const &entry: iter) {
                    // Determine which channel this belongs to.
                    switch (entry.BusChannel) {
                        case 1:
                            exporter_1.writeRecord(entry);
                            ++recordsInChannel_1;
                            break;
                        case 2:
                            exporter_2.writeRecord(entry);
                            ++recordsInChannel_2;
                            break;
                        default:
                            break;
                    }

                    indicator.update(recordsInChannel_1 + recordsInChannel_2);

                }
                indicator.end();

                // If any of the channels are empty, delete the file.
                if (recordsInChannel_1 == 0) {
                    try {
                        output_1.close();
                        bfs::remove(outputFilePathCAN_1);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
                    }
                } else {
                    exporter_1.correctHeader();
                }

                if (recordsInChannel_2 == 0) {
                    try {
                        output_2.close();
                        bfs::remove(outputFilePathCAN_2);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
                    }
                } else {
                    exporter_2.correctHeader();
                }
            }

            if (info.LINMessages > 0) {
                std::size_t recordsInChannel_1 = 0;
                std::size_t recordsInChannel_2 = 0;

                // Create an output for the LIN messages for each channel.
                auto const outputFilePathLIN_1 = outputFolder / (outputFileBase + "LIN_1.txt");
                auto const outputFilePathLIN_2 = outputFolder / (outputFileBase + "LIN_2.txt");

                // Create exporters for each channel and perform common setup.
                std::ofstream output_1(outputFilePathLIN_1.string());
                std::ofstream output_2(outputFilePathLIN_2.string());
                CLX000_LIN_Exporter exporter_1(output_1, info, 1, configuration, commonOptions->displayTimeFormat);
                CLX000_LIN_Exporter exporter_2(output_2, info, 2, configuration, commonOptions->displayTimeFormat);

                exporter_1.writeHeader();
                exporter_2.writeHeader();

                // Loop over all data and write to the correct log file.
                auto iter = mdfFile->getLINIterator();

                mdf::tools::shared::ProgressIndicator indicator(0, info.LINMessages, commonOptions->nonInteractiveMode);
                indicator.setPrefix("LIN");
                std::size_t i = 0;

                indicator.begin();
                for (auto const &entry: iter) {
                    // Determine which channel this belongs to.
                    switch (entry.BusChannel) {
                        case 1:
                            exporter_1.writeRecord(entry);
                            ++recordsInChannel_1;
                            break;
                        case 2:
                            exporter_2.writeRecord(entry);
                            ++recordsInChannel_2;
                            break;
                        default:
                            break;
                    }

                    indicator.update(recordsInChannel_1 + recordsInChannel_2);

                }
                indicator.end();



                // If any of the channels are empty, delete the file.
                if (recordsInChannel_1 == 0) {
                    try {
                        output_1.close();
                        bfs::remove(outputFilePathLIN_1);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
                    }
                } else {
                    exporter_1.correctHeader();
                }

                if (recordsInChannel_2 == 0) {
                    try {
                        output_2.close();
                        bfs::remove(outputFilePathLIN_2);
                    } catch (std::exception &e) {
                        // Could not remove file, nothing we can do, just continue.
                        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
                    }
                } else {
                    exporter_2.correctHeader();
                }
            }

            status = true;
        }

        return status;
    }

    void CLX000_Exporter::configureFileParser(boost::program_options::options_description &opts) {
        opts.add(logOptions);
        opts.add(dataFieldsOptions);
    }

    semver::version const& CLX000_Exporter::getVersion() const {
        return Version;
    }

    tools::shared::ParseOptionStatus
    CLX000_Exporter::parseOptions(boost::program_options::variables_map const &result) {
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
