#ifndef MDFSORTER_CAN_DATAFRAME_H
#define MDFSORTER_CAN_DATAFRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct CAN_DataFrame_s {
    // From general MDF properties.
    uint64_t TimeStamp;

    // From general event properties.
    uint8_t BusChannel;

    // CAN logging specific.
    uint32_t ID;
    bool IDE;
    uint8_t DLC;
    uint8_t DataLength;
    bool Dir;
    bool EDL;
    bool BRS;
    uint8_t DataBytes[64];
} CAN_DataFrame_t;

#ifdef __cplusplus
}
#endif

#endif //MDFSORTER_CAN_DATAFRAME_H
