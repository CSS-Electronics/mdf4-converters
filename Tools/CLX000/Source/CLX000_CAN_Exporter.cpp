#include "CLX000_CAN_Exporter.h"

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/ostream.h>
#include <fmt/format.h>

#include "TimeConverter.h"

namespace mdf::tools::clx {

    CLX000_CAN_Exporter::CLX000_CAN_Exporter(
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
        bitRate = (busChannel == 1) ? fileInfo.BitrateCAN1 : fileInfo.BitrateCAN2;
    }

    void CLX000_CAN_Exporter::writeRecord(CAN_DataFrame const &record) {
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
            std::chrono::seconds timeForConversion = std::chrono::duration_cast<std::chrono::seconds>(logTime);
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
        if (configuration.DataFields_type) {
            fmt::print(output,
                       FMT_STRING("{:b}{:s}"),
                       record.IDE,
                       delimiter
            );
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
