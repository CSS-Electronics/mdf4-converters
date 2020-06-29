#ifndef TOOLS_SHARED_PARSEDFILEINO_H
#define TOOLS_SHARED_PARSEDFILEINO_H

#include <chrono>
#include <cstdint>
#include <optional>

#include <neargye/semver.hpp>

#include "FileInfo.h"
#include "Metadata.h"

namespace mdf::tools::shared {

    struct ParsedFileInfo {
        ParsedFileInfo(FileInfo const& fileInfo, MetadataMap const& metadata);

        std::optional<uint32_t> Session;
        std::optional<uint32_t> Split;
        std::optional<uint32_t> LoggerID;
        std::optional<uint32_t> LoggerType;
        std::optional<uint32_t> BitrateCAN1;
        std::optional<uint32_t> BitrateCAN2;
        std::optional<uint32_t> BitrateLIN1;
        std::optional<uint32_t> BitrateLIN2;
        std::optional<std::string> Comment;
        std::optional<semver::version> FW_Version;
        std::optional<semver::version> HW_Version;

        std::chrono::nanoseconds Time;
        int16_t TimezoneOffsetMinutes;
        uint64_t CANMessages;
        uint64_t LINMessages;
    };

}

#endif //TOOLS_SHARED_PARSEDFILEINO_H
