#include "CLX000_CAN_Exporter.h"

#include <chrono>
#include <cmath>
#include <iomanip>

#include "TimeConverter.h"

namespace tools::clx {

    CLX000_CAN_Exporter::CLX000_CAN_Exporter(
            std::ostream &output,
            FileInfo_t const& fileInfo,
            uint8_t busChannel,
            CLX000_Configuration const& configuration,
            tools::shared::DisplayTimeFormat displayLocalTime
        ) : GenericRecordExporter(output),
        fileInfo(fileInfo),
        busChannel(busChannel),
        configuration(configuration),
        delimiter(std::string_view(configuration.ValueSeparator)),
        displayLocalTime(displayLocalTime) {

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
        if(fileInfo.BitrateCAN1 != 0) {
            converter << static_cast<unsigned int>(fileInfo.BitrateCAN1);
        } else {
            converter << "?";
        }
        bitrateCAN1 = converter.str();

        converter.str("");
        if(fileInfo.BitrateCAN2 != 0) {
            converter << static_cast<unsigned int>(fileInfo.BitrateCAN2);
        } else {
            converter << "?";
        }
        bitrateCAN2 = converter.str();

        output << std::boolalpha;

        output << "# Logger type" << separator << static_cast<unsigned int>(fileInfo.LoggerType) << "\n";
        output << "# HW rev" << separator << fileInfo.HW_Version << "\n";
        output << "# FW rev" << separator << fileInfo.FW_Version << "\n";
        output << "# Logger ID" << separator << std::hex << std::setfill('0') << std::setw(8) << fileInfo.LoggerID << std::dec << "\n";
        output << "# Session No." << separator << static_cast<unsigned int>(fileInfo.Session) << "\n";
        output << "# Split No." << separator << static_cast<unsigned int>(fileInfo.Split) << "\n";
        output << "# Time" << separator;

        // Store the current location for later correction of the timestamp.
        output.flush();
        datePosition = output.tellp();

        // Insert spaces for now.
        int const dateLength = convertTimestampWithYear(fileInfo.Time).size();
        for(int i = 0; i < dateLength; ++i) {
            output << " ";
        }
        output << "\n";

        output << "# Value separator" << separator << "\"" << delimiter << "\"" << "\n";
        output << "# Time format" << separator << configuration.TimestampFormat << "\n";
        output << "# Time separator" << separator << "\"" << std::string_view(configuration.TimestampTimeSeparator) << "\"" << "\n";
        output << "# Time separator ms" << separator << "\"" << std::string_view(configuration.TimestampTimeMsSeparator) << "\"" << "\n";
        output << "# Date separator" << separator << "\"" << std::string_view(configuration.TimestampDateSeparator) << "\"" << "\n";
        output << "# Time and date separator" << separator << "\"" << std::string_view(configuration.TimeTimeDateSeparator) << "\"" << "\n";

        if(busChannel == 1) {
            output << "# Bit-rate" << separator << bitrateCAN1 << "\n";
        } else {
            output << "# Bit-rate" << separator << bitrateCAN2 << "\n";
        }

        output << "# Silent mode" << separator << false << "\n";
        output << "# Cyclic mode" << separator << true << "\n";

        // Write the column headers.
        if(configuration.DataFields_timestamp) {
            output << "Timestamp" << delimiter;
        }
        if(configuration.DataFields_type) {
            output << "Type" << delimiter;
        }
        if(configuration.DataFields_id) {
            output << "ID" << delimiter;
        }
        if(configuration.DataFields_dataLength) {
            output << "Length" << delimiter;
        }
        if(configuration.DataFields_data) {
            output << "Data";
        }
        output << "\n";

        output << std::noboolalpha;
    }

    void CLX000_CAN_Exporter::writeRecord(CAN_DataFrame_t record) {
        if(!timeStampSet) {
            timeStampSet = true;
            headerTimeStamp = static_cast<time_t>(record.TimeStamp * 1E-9);
        }

        if(configuration.DataFields_timestamp) {
            // Convert to right zone.
            std::chrono::nanoseconds logTime(record.TimeStamp);
            std::chrono::seconds timeForConversion = std::chrono::duration_cast<std::chrono::seconds>(logTime);
            std::chrono::nanoseconds floatingPart = logTime - timeForConversion;

            std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, static_cast<time_t>(timeForConversion.count()), fileInfo);

            // Shift from seconds to nanoseconds again.
            correctedTime *= 1000000000;

            // Add the nanoseconds, which were removed earlier.
            correctedTime += floatingPart.count();

            output << convertTimestampToFormat(correctedTime) << delimiter;
        }
        if(configuration.DataFields_type) {
            if(record.IDE) {
                output << "1" << delimiter;
            } else {
                output << "0" << delimiter;
            }
        }

        // The rest of the record is in lowercase hex.
        output << std::hex;

        if(configuration.DataFields_id) {
            output << record.ID << delimiter;
        }
        if(configuration.DataFields_dataLength) {
            output << static_cast<int>(record.DataLength) << delimiter;
        }
        if(configuration.DataFields_data) {
            output << std::hex << std::setfill('0');

            for(int i = 0; i < record.DataLength; ++i) {
                output << std::setw(2) << static_cast<int>(record.DataBytes[i]);
            }
        }

        // Reset format.
        output << std::dec << "\n";
    }

    std::string CLX000_CAN_Exporter::convertTimestampWithYear(std::time_t const& timeStamp) {
        // Reset buffer.
        timeStampStream.str("");

        // Convert to right zone.
        std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

        // Convert epoch to datetime.
        std::tm const* tm = std::gmtime(&correctedTime);

        timeStampStream << std::put_time(tm, "%Y%m%dT%H%M%S");
        return timeStampStream.str();
    }

    std::string CLX000_CAN_Exporter::convertTimestampToFormat(std::time_t const& timeStamp) {
        // Reset label.
        timeStream.str("");

        // Select correct conversion.
        switch (configuration.TimestampFormat) {
            case 0:
                convertTimestampToFormat0(timeStamp);
                break;
            case 1:
                convertTimestampToFormat1(timeStamp);
                break;
            case 2:
                convertTimestampToFormat2(timeStamp);
                break;
            case 3:
                convertTimestampToFormat3(timeStamp);
                break;
            case 4:
                convertTimestampToFormat4(timeStamp);
                break;
            case 5:
                convertTimestampToFormat5(timeStamp);
                break;
            case 6:
                convertTimestampToFormat6(timeStamp);
                break;
            default:
                break;
        }

        return timeStream.str();
    }

    void CLX000_CAN_Exporter::correctHeader() {
        if(timeStampSet) {
            output.flush();
            output.seekp(datePosition);
            output << convertTimestampWithYear(headerTimeStamp);
        }
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat0(std::time_t const& timeStamp) {
        // Format is just the millisceonds.
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        timeStream << std::setfill('0') << std::setw(3) << std::fixed << std::setprecision(0);
        timeStream << floatingPart * 1E3;
        timeStream.unsetf(std::ios_base::fixed);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat1(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%S");
        timeStream << std::string_view(configuration.TimestampTimeMsSeparator);
        convertTimestampToFormat0(timeStamp);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat2(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%M");
        timeStream << std::string_view(configuration.TimestampTimeSeparator);
        convertTimestampToFormat1(timeStamp);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat3(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%H");
        timeStream << std::string_view(configuration.TimestampTimeSeparator);
        convertTimestampToFormat2(timeStamp);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat4(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%d");
        timeStream << std::string_view(configuration.TimeTimeDateSeparator);
        convertTimestampToFormat3(timeStamp);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat5(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%m");
        timeStream << std::string_view(configuration.TimestampDateSeparator);
        convertTimestampToFormat4(timeStamp);
    }

    void CLX000_CAN_Exporter::convertTimestampToFormat6(std::time_t const& timeStamp) {
        double mainPart, floatingPart;
        floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

        time_t mainTimeStamp = floor(mainPart);
        timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%Y");
        timeStream << std::string_view(configuration.TimestampDateSeparator);
        convertTimestampToFormat5(timeStamp);
    }

}
