#ifndef MDFSORTER_MDF_FILE_H
#define MDFSORTER_MDF_FILE_H

#include "CAN_DataFrame.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Close and release the resources associated with an opened MDF file.
 * @param fileHandle    The handle returned by mdf_openFile.
 */
void mdf_closeFile(void* fileHandle);

/**
 * Iterate over the CAN frames in the MDF file.
 *
 * @param fileHandle    Handle to an opened MDF file.
 * @param record        Location to store the information regarding the CAN frame.
 * @return              True if a frame was read, false otherwise.
 */
bool mdf_getNextCANDataFrame(void* fileHandle, CAN_DataFrame_t* record);

/**
 * Attempt to open a MDF4 file.
 * Will perform in memory finalization and sorting.
 *
 * @param fileName      Path to the MDF file.
 * @return              Handle to the opened file, NULL in case of error.
 */
void* mdf_openFile(char const* fileName);

#ifdef __cplusplus
}
#endif

#endif //MDFSORTER_MDF_FILE_H
