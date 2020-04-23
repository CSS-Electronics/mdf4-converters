#ifndef MDFSIMPLECONVERTERS_EXTRACTMETADATA_H
#define MDFSIMPLECONVERTERS_EXTRACTMETADATA_H

#include "FileInfo.h"

namespace mdf {

  /*! Extract information specific to the CAN channels.
   *
   * @param fileInfo      Information structure to save any loaded data in.
   * @param xml           XML data to parse.
   * @return              false if an error occurred.
   */
  bool getCANInfo(FileInfo& fileInfo, std::string xml);

  /*!Extract information specific to the LIN channels.
   *
   * @param fileInfo      Information structure to save any loaded data in.
   * @param xml           XML data to parse.
   * @return              false if an error occurred.
   */
  bool getLINInfo(FileInfo& fileInfo, std::string xml);

  /*!Extract information shared among the channels.
   *
   * @param fileInfo      Information structure to save any loaded data in.
   * @param xml           XML data to parse.
   * @return              false if an error occurred.
   */
  bool getSharedInfo(FileInfo& fileInfo, std::string xml);

}

#endif //MDFSIMPLECONVERTERS_EXTRACTMETADATA_H
