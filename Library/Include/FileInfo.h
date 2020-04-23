#ifndef MDFSIMPLECONVERTERS_FILEINFO_H
#define MDFSIMPLECONVERTERS_FILEINFO_H

#include <chrono>
#include <cstdint>

#include "Version.h"

namespace mdf {

  struct FileInfo {
    FileInfo();

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
    uint64_t TimezoneOffset;

    Version FW_Version;
    Version HW_Version;
    uint64_t CANMessages;
    uint64_t LINMessages;
  };

}

#endif //MDFSIMPLECONVERTERS_FILEINFO_H
