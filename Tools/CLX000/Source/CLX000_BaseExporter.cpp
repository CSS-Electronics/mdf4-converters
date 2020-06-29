#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "CLX000_BaseExporter.h"

#include "CAN_DataFrame.h"
#include "LIN_Frame.h"
#include "TimeConverter.h"

namespace mdf::tools::clx {

    template<typename T>
    CLX000_BaseExporter<T>::CLX000_BaseExporter(
        std::ostream& output,
        mdf::tools::shared::ParsedFileInfo const& fileInfo,
        uint8_t busChannel,
        CLX000_Configuration const& configuration,
        shared::DisplayTimeFormat displayLocalTime
    ) : mdf::tools::shared::GenericRecordExporter<T>(output),
        fileInfo(fileInfo),
        busChannel(busChannel),
        configuration(configuration),
        delimiter(std::string_view(configuration.ValueSeparator)),
        displayLocalTime(displayLocalTime)
        {

    }

    template<typename T>
    void CLX000_BaseExporter<T>::writeHeader() {
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

        // Logger type.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Logger type{:s}{:d}\n"),
            separator,
            fileInfo.LoggerType.value_or(0)
        );

        // Hardware version.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# HW rev{:s}{:02d}.{:02d}\n"),
            separator,
            fileInfo.HW_Version.value_or(semver::version("0.0.0")).major,
            fileInfo.HW_Version.value_or(semver::version("0.0.0")).minor
        );

        // Firmware version.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# FW rev{:s}{:02d}.{:02d}.{:02d}\n"),
            separator,
            fileInfo.FW_Version.value_or(semver::version("0.0.0")).major,
            fileInfo.FW_Version.value_or(semver::version("0.0.0")).minor,
            fileInfo.FW_Version.value_or(semver::version("0.0.0")).patch
        );

        // Logger ID.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Logger ID{:s}{:08x}\n"),
            separator,
            fileInfo.LoggerID.value_or(0)
        );

        // File session.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Session No.{:s}{:d}\n"),
            separator,
            fileInfo.Session.value_or(0)
        );

        // File split.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Split No.{:s}{:d}\n"),
            separator,
            fileInfo.Split.value_or(0)
        );

        // Time.
        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Time{:s}"),
            separator
        );

        // Store the current location for later correction of the timestamp.
        mdf::tools::shared::GenericRecordExporter<T>::output.flush();
        datePosition = mdf::tools::shared::GenericRecordExporter<T>::output.tellp();

        // Insert the default time for now.
        auto const fileTime = std::chrono::duration_cast<std::chrono::seconds>(fileInfo.Time);
        convertTimestampWithYear(fileTime.count(), mdf::tools::shared::GenericRecordExporter<T>::output);
        fmt::print(mdf::tools::shared::GenericRecordExporter<T>::output, FMT_STRING("\n"));

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Value separator{:s}\"{:s}\"\n"),
            separator,
            delimiter
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Time format{:s}{:d}\n"),
            separator,
            configuration.TimestampFormat
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Time separator{:s}\"{:s}\"\n"),
            separator,
            configuration.TimestampTimeSeparator
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Time separator ms{:s}\"{:s}\"\n"),
            separator,
            configuration.TimestampTimeMsSeparator
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Date separator{:s}\"{:s}\"\n"),
            separator,
            configuration.TimestampDateSeparator
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Time and date separator{:s}\"{:s}\"\n"),
            separator,
            configuration.TimeTimeDateSeparator
        );

        std::string bitrateString = "?";
        if(bitRate) {
            bitrateString = fmt::format(FMT_STRING("{:d}"), bitRate.value());
        }

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Bit-rate{:s}{:s}\n"),
            separator,
            bitrateString
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Silent mode{:s}{}\n"),
            separator,
            false
        );

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("# Cyclic mode{:s}{}\n"),
            separator,
            true
        );

        // Write the column headers.
        std::vector<std::string_view> headers;
        if (configuration.DataFields_timestamp) {
            headers.emplace_back("Timestamp");
        }
        if (configuration.DataFields_type) {
            headers.emplace_back("Type");
        }
        if (configuration.DataFields_id) {
            headers.emplace_back("ID");
        }
        if (configuration.DataFields_dataLength) {
            headers.emplace_back("Length");
        }
        if (configuration.DataFields_data) {
            headers.emplace_back("Data");
        }

        fmt::print(
            mdf::tools::shared::GenericRecordExporter<T>::output,
            FMT_STRING("{}\n"),
            fmt::join(headers, delimiter)
        );
    }

    template<typename T>
    void CLX000_BaseExporter<T>::convertTimestampWithYear(std::time_t const &timeStamp, std::ostream &output) {
        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

        // Convert epoch to datetime.
        std::tm const tm = *std::gmtime(&correctedTime);

        fmt::print(output,
                   "{:%Y%m%dT%H%M%S}",
                   tm
        );
    }

    template<typename T>
    void CLX000_BaseExporter<T>::convertTimestampToFormat(std::chrono::nanoseconds const& timeStamp, std::ostream& output) {
        auto const secondsComponent = std::chrono::duration_cast<std::chrono::seconds>(timeStamp);
        auto const millisComponent = std::chrono::round<std::chrono::milliseconds>(timeStamp - secondsComponent);
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

    template<typename T>
    void CLX000_BaseExporter<T>::correctHeader() {
        if (timeStampSet) {
            mdf::tools::shared::GenericRecordExporter<T>::output.flush();
            mdf::tools::shared::GenericRecordExporter<T>::output.seekp(datePosition);
            convertTimestampWithYear(headerTimeStamp, mdf::tools::shared::GenericRecordExporter<T>::output);
        }
    }

    // Instantiate the required templates.
    template
    struct CLX000_BaseExporter<mdf::CAN_DataFrame>;

    template
    struct CLX000_BaseExporter<mdf::LIN_Frame>;
}
