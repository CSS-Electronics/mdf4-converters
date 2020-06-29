#include "CSV_Exporter.h"
#include "CSV_CAN_Exporter.h"
#include "CSV_LIN_Exporter.h"
#include "MdfFile.h"
#include "ProgressIndicator.h"
#include "Version.h"

using namespace mdf;

namespace mdf::tools::csv {

    CSV_Exporter::CSV_Exporter() : ConverterInterface(PROGRAM_NAME) {

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

            if (info.CANMessages > 0) {
                // Create an output for the CAN messages.
                auto outputFilePathCAN = outputFolder / (outputFileBase + "CAN.csv");
                std::ofstream output(outputFilePathCAN.string());

                // Create a CAN exporter.
                CSV_CAN_Exporter exporter(output);

                // Write the header.
                exporter.writeHeader();

                auto iter_a = mdfFile->getCANIterator();
                auto iter_b = mdfFile->getCANRemoteFrameIterator();

                // Create temporary storage for the sorted results.
                std::vector<CSV_Record> result;

                auto CustomCompare = [](CSV_Record const& a, CSV_Record const& b)
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

                indicator.begin();
                for (auto const &entry: result) {
                    exporter.writeRecord(entry);
                    indicator.update(++i);
                }
                indicator.end();
            }

            if (info.LINMessages > 0) {
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

    semver::version const& CSV_Exporter::getVersion() const {
        return Version;
    }

}
