#ifndef TOOLS_SHARED_PARSEDFILEINO_H
#define TOOLS_SHARED_PARSEDFILEINO_H

#include <chrono>
#include <cstdint>

#include "FileInfo.h"
#include "Metadata.h"
#include "Version.h"

namespace mdf::tools::shared {

    struct ParsedFileInfo {
        ParsedFileInfo(FileInfo const& fileInfo, MetadataMap const& metadata);

        uint32_t Session;
        uint32_t Split;
        uint32_t LoggerID;
        uint32_t LoggerType;
        uint32_t BitrateCAN1;
        uint32_t BitrateCAN2;
        uint32_t BitrateLIN1;
        uint32_t BitrateLIN2;
        char Comment[30 + 1];

        std::chrono::nanoseconds Time;
        int16_t TimezoneOffsetMinutes;

        Version FW_Version;
        Version HW_Version;
        uint64_t CANMessages;
        uint64_t LINMessages;
    };

}

#endif //TOOLS_SHARED_PARSEDFILEINO_H
