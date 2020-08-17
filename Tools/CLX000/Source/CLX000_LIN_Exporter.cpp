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
  ) : GenericRecordExporter(output),
      fileInfo(fileInfo),
      busChannel(busChannel),
      configuration(configuration),
      delimiter(std::string_view(configuration.ValueSeparator)),
      displayLocalTime(displayLocalTime) {

  }

  void CLX000_LIN_Exporter::writeHeader() {
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

    std::string bitrateLIN1;
    std::string bitrateLIN2;

    converter.str("");
    if (fileInfo.BitrateLIN1 != 0) {
      converter << static_cast<unsigned int>(fileInfo.BitrateLIN1);
    } else {
      converter << "?";
    }
    bitrateLIN1 = converter.str();

    converter.str("");
    if (fileInfo.BitrateLIN2 != 0) {
      converter << static_cast<unsigned int>(fileInfo.BitrateLIN2);
    } else {
      converter << "?";
    }
    bitrateLIN2 = converter.str();

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

    // Insert spaces for now.
    int const dateLength = convertTimestampWithYear(
      std::chrono::duration_cast<std::chrono::seconds>(fileInfo.Time).count()).size();
    for (int i = 0; i < dateLength; ++i) {
      output << " ";
    }
    output << "\n";

    output << "# Value separator" << separator << "\"" << delimiter << "\"" << "\n";
    output << "# Time format" << separator << configuration.TimestampFormat << "\n";
    output << "# Time separator" << separator << "\"" << std::string_view(configuration.TimestampTimeSeparator) << "\""
           << "\n";
    output << "# Time separator ms" << separator << "\"" << std::string_view(configuration.TimestampTimeMsSeparator)
           << "\"" << "\n";
    output << "# Date separator" << separator << "\"" << std::string_view(configuration.TimestampDateSeparator) << "\""
           << "\n";
    output << "# Time and date separator" << separator << "\"" << std::string_view(configuration.TimeTimeDateSeparator)
           << "\"" << "\n";

    if (busChannel == 1) {
      output << "# Bit-rate" << separator << bitrateLIN1 << "\n";
    } else {
      output << "# Bit-rate" << separator << bitrateLIN2 << "\n";
    }

    output << "# Silent mode" << separator << false << "\n";
    output << "# Cyclic mode" << separator << true << "\n";

    // Write the column headers.
    if (configuration.DataFields_timestamp) {
      output << "Timestamp" << delimiter;
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

  void CLX000_LIN_Exporter::writeRecord(LINRecord const &record) {
    if (!timeStampSet) {
      timeStampSet = true;
      headerTimeStamp = std::chrono::duration_cast<std::chrono::seconds>(record.TimeStamp).count();
    }

    if (configuration.DataFields_timestamp) {
      // Convert to right zone.
      std::chrono::nanoseconds logTime(record.TimeStamp);
      std::chrono::seconds timeForConversion = std::chrono::duration_cast<std::chrono::seconds>(logTime);
      std::chrono::nanoseconds floatingPart = logTime - timeForConversion;

      std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime,
        static_cast<time_t>(timeForConversion.count()), fileInfo);

      // Shift from seconds to nanoseconds again.
      correctedTime *= 1000000000;

      // Add the nanoseconds, which were removed earlier.
      correctedTime += floatingPart.count();

      fmt::print(output,
        FMT_STRING("{:s}{:s}"),
        convertTimestampToFormat(correctedTime),
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

  std::string CLX000_LIN_Exporter::convertTimestampWithYear(std::time_t const &timeStamp) {
    // Reset buffer.
    timeStampStream.str("");

    // Convert to right zone.
    std::time_t correctedTime = tools::shared::convertTimeStamp(displayLocalTime, timeStamp, fileInfo);

    // Convert epoch to datetime.
    std::tm const *tm = std::gmtime(&correctedTime);

    timeStampStream << std::put_time(tm, "%Y%m%dT%H%M%S");
    return timeStampStream.str();
  }

  std::string CLX000_LIN_Exporter::convertTimestampToFormat(std::time_t const &timeStamp) {
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

  void CLX000_LIN_Exporter::correctHeader() {
    if (timeStampSet) {
      output.flush();
      output.seekp(datePosition);
      output << convertTimestampWithYear(headerTimeStamp);
    }
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat0(std::time_t const &timeStamp) {
    // Format is just the milliseconds.
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    int truncatedFloatingPart = std::lround(floatingPart * 1E3);

    if (truncatedFloatingPart >= 1000) {
      truncatedFloatingPart = 0;
    }

    timeStream << std::setfill('0') << std::setw(3);
    timeStream << truncatedFloatingPart;
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat1(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);
    time_t mainTimeStamp = floor(mainPart);

    // Special handle for the case of 1000 ms.
    int truncatedFloatingPart = std::lround(floatingPart * 1E3);

    if (truncatedFloatingPart >= 1000) {
      mainTimeStamp += 1;
    }

    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%S");
    timeStream << std::string_view(configuration.TimestampTimeMsSeparator);

    convertTimestampToFormat0(timeStamp);
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat2(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    time_t mainTimeStamp = floor(mainPart);
    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%M");
    timeStream << std::string_view(configuration.TimestampTimeSeparator);
    convertTimestampToFormat1(timeStamp);
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat3(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    time_t mainTimeStamp = floor(mainPart);
    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%H");
    timeStream << std::string_view(configuration.TimestampTimeSeparator);
    convertTimestampToFormat2(timeStamp);
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat4(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    time_t mainTimeStamp = floor(mainPart);
    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%d");
    timeStream << std::string_view(configuration.TimeTimeDateSeparator);
    convertTimestampToFormat3(timeStamp);
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat5(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    time_t mainTimeStamp = floor(mainPart);
    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%m");
    timeStream << std::string_view(configuration.TimestampDateSeparator);
    convertTimestampToFormat4(timeStamp);
  }

  void CLX000_LIN_Exporter::convertTimestampToFormat6(std::time_t const &timeStamp) {
    double mainPart, floatingPart;
    floatingPart = std::modf(timeStamp * 1E-9, &mainPart);

    time_t mainTimeStamp = floor(mainPart);
    timeStream << std::put_time(std::gmtime(&mainTimeStamp), "%Y");
    timeStream << std::string_view(configuration.TimestampDateSeparator);
    convertTimestampToFormat5(timeStamp);
  }

}
