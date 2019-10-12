#ifndef MDFSORTER_EXTRACTMETADATA_H
#define MDFSORTER_EXTRACTMETADATA_H

#include <string>

#include "FileInfo.h"

namespace mdf {

    /**
     * Extract information specific to the CAN channels.
     *
     * @param fileInfo      Information structure to save any loaded data in.
     * @param xml           XML data to parse.
     * @return              false if an error occurred.
     */
    bool getCANInfo(FileInfo_t& fileInfo, std::string xml);

    /**
     * Extract information specific to the LIN channels.
     *
     * @param fileInfo      Information structure to save any loaded data in.
     * @param xml           XML data to parse.
     * @return              false if an error occurred.
     */
    bool getLINInfo(FileInfo_t& fileInfo, std::string xml);

    /**
     * Extract information shared among the channels.
     *
     * @param fileInfo      Information structure to save any loaded data in.
     * @param xml           XML data to parse.
     * @return              false if an error occurred.
     */
    bool getSharedInfo(FileInfo_t& fileInfo, std::string xml);

}

#endif //MDFSORTER_EXTRACTMETADATA_H
