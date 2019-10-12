#ifndef MDFSORTER_FILEINFO_H
#define MDFSORTER_FILEINFO_H

#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#include "SemanticVersion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FileInfo_s {
    uint32_t Session;
    uint32_t Split;
    uint32_t LoggerID;
    uint32_t LoggerType;
    uint32_t BitrateCAN1;
    uint32_t BitrateCAN2;
    uint32_t BitrateLIN1;
    uint32_t BitrateLIN2;
    char Comment[30 + 1];
    time_t Time;
    time_t TimezoneOffset;
    SemanticVersion_t FW_Version;
    SemanticVersionLimited_t HW_Version;
    uint64_t CANMessages;
    uint64_t LINMessages;
} FileInfo_t;

#ifdef __cplusplus
}
#endif

#endif //MDFSORTER_FILEINFO_H
