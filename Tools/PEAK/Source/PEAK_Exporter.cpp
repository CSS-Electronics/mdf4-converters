#include <algorithm>
#include <fstream>
#include <variant>
#include <vector>

#include "PEAK_Exporter.h"

#include "MdfFile.h"
#include "PEAK_CAN_Exporter.h"
#include "PEAK_CAN_Exporter_1_1.h"
#include "PEAK_CAN_Exporter_2_1.h"
#include "ProgressIndicator.h"
#include "Version.h"

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
                    auto iter_a = mdfFile->getCANIterator();
                    auto iter_b = mdfFile->getCANRemoteFrameIterator();

                    // Create temporary storage for the sorted results.
                    std::vector<PEAK_Record> result;

                    auto CustomCompare = [](PEAK_Record const& a, PEAK_Record const& b)
                    {
                        std::chrono::nanoseconds timeStamp_a;
                        std::chrono::nanoseconds timeStamp_b;

                        auto compareFunc = [](auto&& arg) {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, mdf::CAN_DataFrame>) {
                                return arg.TimeStamp;
                            } else if constexpr (std::is_same_v<T, mdf::CAN_RemoteFrame>) {
                                return arg.TimeStamp;
                            } else {
                                static_assert(always_false_v < T > , "Missing visitor");
                            }
                        };

                        timeStamp_a = std::visit(compareFunc, a);
                        timeStamp_b = std::visit(compareFunc, b);

                        return timeStamp_a < timeStamp_b;
                    };

                    std::merge(
                        iter_a.cbegin(),
                        iter_a.cend(),
                        iter_b.cbegin(),
                        iter_b.cend(),
                        std::back_inserter(result),
                        CustomCompare
                        );

                    mdf::tools::shared::ProgressIndicator indicator(
                        0,
                        result.size(),
                        commonOptions->nonInteractiveMode
                        );
                    indicator.setPrefix("CAN");
                    std::size_t i = 0;

                    // Write all records.
                    indicator.begin();
                    for (auto const &entry: result) {
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

    semver::version const& PEAK_Exporter::getVersion() const {
        return Version;
    }

    void PEAK_Exporter::configureParser(boost::program_options::options_description &opts) {
        // Add option for selecting which version to target.
        opts.add_options()
            ("trace-format,f", boost::program_options::value<PEAK_TraceFormat>(&traceFormat)->default_value(
                PEAK_TraceFormat::VERSION_2_1),
             "select which version to target");
    }

}
