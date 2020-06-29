#include "CLX000_LIN_Exporter.h"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <fmt/ostream.h>

#include "TimeConverter.h"

namespace mdf::tools::clx {

    CLX000_LIN_Exporter::CLX000_LIN_Exporter(
        std::ostream &output,
        mdf::tools::shared::ParsedFileInfo const &fileInfo,
        uint8_t busChannel,
        CLX000_Configuration const &configuration,
        tools::shared::DisplayTimeFormat displayLocalTime
    ) : CLX000_BaseExporter(
        output,
        fileInfo,
        busChannel,
        configuration,
        displayLocalTime
    ) {
        bitRate = (busChannel == 1) ? fileInfo.BitrateLIN1 : fileInfo.BitrateLIN2;
    }

    void CLX000_LIN_Exporter::writeRecord(LIN_Frame const &record) {
        if (!timeStampSet) {
            timeStampSet = true;
            headerTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(record.TimeStamp).count();
        }

        if (configuration.DataFields_timestamp) {
            convertTimestampToFormat(record.TimeStamp, output);
            fmt::print(output,
                       FMT_STRING("{:s}"),
                       delimiter
            );

            /*
            // Convert to right zone.
            std::chrono::nanoseconds logTime(record.TimeStamp);
            auto const timeForConversion = std::chrono::duration_cast<std::chrono::seconds>(logTime);
            std::chrono::nanoseconds floatingPart = logTime - timeForConversion;

            std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime,
                                                                        static_cast<time_t>(timeForConversion.count()),
                                                                        fileInfo);

            // Shift from seconds to nanoseconds again.
            correctedTime *= 1000000000;

            // Add the nanoseconds, which were removed earlier.
            correctedTime += floatingPart.count();

            fmt::print(output,
                       FMT_STRING("{:s}{:s}"),
                       convertTimestampToFormat(correctedTime),
                       delimiter
            );*/
        }

        if (configuration.DataFields_id) {
            fmt::print(output,
                       FMT_STRING("{:x}{:s}"),
                       record.ID,
                       delimiter
            );
        }
        if (configuration.DataFields_dataLength) {
            fmt::print(output,
                       FMT_STRING("{:x}{:s}"),
                       record.DataLength,
                       delimiter
            );
        }
        if (configuration.DataFields_data) {
            fmt::print(output,
                       FMT_STRING("{:02x}"),
                       fmt::join(record.DataBytes, "")
            );
        }

        fmt::print(output, "\n");
    }

}
