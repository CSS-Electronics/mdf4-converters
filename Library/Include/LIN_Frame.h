#ifndef MDFSORTER_LIN_FRAME_H
#define MDFSORTER_LIN_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct LIN_Frame_s {
    // From general MDF properties.
    uint64_t TimeStamp;

    // From general event properties.
    uint8_t BusChannel;

    // CAN logging specific.
    uint32_t ID;
    bool Dir;
    uint8_t ReceivedDataByteCount;
    uint8_t DataLength;
    uint8_t DataBytes[8];
} LIN_Frame_t;

#ifdef __cplusplus
}
#endif

#endif //MDFSORTER_LIN_FRAME_H
