#include "CLX000_CAN_Exporter.h"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <fmt/chrono.h>
#include <fmt/ostream.h>
#include <fmt/format.h>

#include "TimeConverter.h"

namespace mdf::tools::clx {

    CLX000_CAN_Exporter::CLX000_CAN_Exporter(
        std::ostream &output,
        FileInfo const &fileInfo,
        uint8_t busChannel,
        CLX000_Configuration const &configuration,
        tools::shared::DisplayTimeFormat displayLocalTime
    ) : GenericRecordExporter(output),
        fileInfo(fileInfo),
        busChannel(busChannel),
        configuration(configuration),
        delimiter(std::string_view(configuration.ValueSeparator)),
        displayLocalTime(displayLocalTime) {
        //
    }

    void CLX000_CAN_Exporter::writeHeader() {
        // Header, containing:
        // Logger hardware type (Logger type)
        // Hardware version (HW rev)
        // Firmware version (FW rev)
        // Logger ID (Logger ID)
        // Session number (Session No.)
        // Split number (Split)
        // Time of file creation
        // Value separator (Value separator)
        // Time format in old logger code (Time format)
        // Time separator (Time separator)
        // Time separator ms (Time separator ms)
        // Date separator (Date separator)
        // Time and date separator (Time and date separator)
        // Bit rate (Bit-rate)
        // Silent operation (Silent mode)
        // Cyclic logging (Cyclic mode)
        std::string const separator = ": ";

        // Convert the bit rates to numbers or "?".
        std::stringstream converter;

        std::string bitrateCAN1;
        std::string bitrateCAN2;

        converter.str("");
        if (fileInfo.BitrateCAN1 != 0) {
            converter << static_cast<unsigned int>(fileInfo.BitrateCAN1);
        } else {
            converter << "?";
        }
        bitrateCAN1 = converter.str();

        converter.str("");
        if (fileInfo.BitrateCAN2 != 0) {
            converter << static_cast<unsigned int>(fileInfo.BitrateCAN2);
        } else {
            converter << "?";
        }
        bitrateCAN2 = converter.str();

        output << std::boolalpha;

        output << "# Logger type" << separator << static_cast<unsigned int>(fileInfo.LoggerType) << "\n";
        output << "# HW rev" << separator << fileInfo.HW_Version << "\n";
        output << "# FW rev" << separator << fileInfo.FW_Version << "\n";
        output << "# Logger ID" << separator << std::hex << std::setfill('0') << std::setw(8) << fileInfo.LoggerID
               << std::dec << "\n";
        output << "# Session No." << separator << static_cast<unsigned int>(fileInfo.Session) << "\n";
        output << "# Split No." << separator << static_cast<unsigned int>(fileInfo.Split) << "\n";
        output << "# Time" << separator;

        // Store the current location for later correction of the timestamp.
        output.flush();
        datePosition = output.tellp();

        // Insert the default time for now.
        std::chrono::seconds const fileTime = std::chrono::duration_cast<std::chrono::seconds>(fileInfo.Time);
        convertTimestampWithYear(fileTime.count(), output);
        output << "\n";

        output << "# Value separator" << separator << "\"" << delimiter << "\"" << "\n";
        output << "# Time format" << separator << configuration.TimestampFormat << "\n";
        output << "# Time separator" << separator << "\"" << std::string_view(configuration.TimestampTimeSeparator)
               << "\""
               << "\n";
        output << "# Time separator ms" << separator << "\"" << std::string_view(configuration.TimestampTimeMsSeparator)
               << "\"" << "\n";
        output << "# Date separator" << separator << "\"" << std::string_view(configuration.TimestampDateSeparator)
               << "\""
               << "\n";
        output << "# Time and date separator" << separator << "\""
               << std::string_view(configuration.TimeTimeDateSeparator)
               << "\"" << "\n";

        if (busChannel == 1) {
            output << "# Bit-rate" << separator << bitrateCAN1 << "\n";
        } else {
            output << "# Bit-rate" << separator << bitrateCAN2 << "\n";
        }

        output << "# Silent mode" << separator << false << "\n";
        output << "# Cyclic mode" << separator << true << "\n";

        // Write the column headers.
        if (configuration.DataFields_timestamp) {
            output << "Timestamp" << delimiter;
        }
        if (configuration.DataFields_type) {
            output << "Type" << delimiter;
        }
        if (configuration.DataFields_id) {
            output << "ID" << delimiter;
        }
        if (configuration.DataFields_dataLength) {
            output << "Length" << delimiter;
        }
        if (configuration.DataFields_data) {
            output << "Data";
        }
        output << "\n";

        output << std::noboolalpha;
    }

    void CLX000_CAN_Exporter::writeRecord(CANRecord const &record) {
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

    void CLX000_CAN_Exporter::convertTimestampWithYear(std::time_t const &timeStamp, std::ostream &output) {
        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

        // Convert epoch to datetime.
        std::tm const tm = *std::gmtime(&correctedTime);

        fmt::print(output,
                   "{:%Y%m%dT%H%M%S}",
                   tm
        );
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat(std::chrono::nanoseconds const& timeStamp, std::ostream& output) {
        std::chrono::seconds const secondsComponent = std::chrono::duration_cast<std::chrono::seconds>(timeStamp);
        std::chrono::milliseconds const millisComponent = std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp - secondsComponent);
        std::time_t const secondsComponentTime = secondsComponent.count();

        std::tm const genericTimeStamp = *std::gmtime(&secondsComponentTime);

        // NOTE: When using std::tm for date-time output, compile time checks on the format string cannot be
        //       performed. See https://github.com/fmtlib/fmt/issues/1269

        // The different timestamp formats are inclusive. Check for the longest edition, and filter down.
        switch (configuration.TimestampFormat) {
            case 6:
                fmt::print(output,
                           "{:%Y}{:s}",
                           genericTimeStamp,
                           configuration.TimestampDateSeparator
                );
                // Fallthrough
            case 5:
                fmt::print(output,
                           "{:%m}{:s}",
                           genericTimeStamp,
                           configuration.TimestampDateSeparator
                );
                // Fallthrough
            case 4:
                fmt::print(output,
                           "{:%d}{:s}",
                           genericTimeStamp,
                           configuration.TimeTimeDateSeparator
                );
                // Fallthrough
            case 3:
                fmt::print(output,
                           "{:%H}{:s}",
                           genericTimeStamp,
                           configuration.TimestampTimeSeparator
                );
                // Fallthrough
            case 2:
                fmt::print(output,
                           "{:%M}{:s}",
                           genericTimeStamp,
                           configuration.TimestampTimeSeparator
                );
                // Fallthrough
            case 1:
                fmt::print(output,
                           "{:%S}{:s}",
                           genericTimeStamp,
                           configuration.TimestampTimeMsSeparator
                );
                // Fallthrough
            case 0:
                fmt::print(output,
                           FMT_STRING("{:03d}"),
                           millisComponent.count()
                );
                break;
            default:
                break;
        }
    }

    void CLX000_CAN_Exporter::correctHeader() {
        if (timeStampSet) {
            output.flush();
            output.seekp(datePosition);
            convertTimestampWithYear(headerTimeStamp, output);
        }
    }

}
