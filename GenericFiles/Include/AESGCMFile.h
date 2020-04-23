#ifndef MDFSIMPLECONVERTERS_AESGCMFILE_H
#define MDFSIMPLECONVERTERS_AESGCMFILE_H

#include <cstdint>
#include <string>

namespace mdf {

  /*!
   * Examine the header of a file to determine if it is encrypted.
   * @param fileName
   * @return
   */
  bool isEncryptedFile(std::string const &fileName);

  /*!
   * Decrypt the given file using a password.
   * @param inputFile
   * @param outputFile
   * @param password
   * @return
   */
  bool decryptFile(std::string const &inputFile, std::string const &outputFile, std::string const &password);

  /*!
   *
   * @param fileName
   * @return
   */
  uint32_t getDeviceIdFromFile(std::string const &fileName);
}

#endif //MDFSIMPLECONVERTERS_AESGCMFILE_H
