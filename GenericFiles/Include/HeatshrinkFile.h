#ifndef MDFSIMPLECONVERTERS_HEATSHRINKFILE_H
#define MDFSIMPLECONVERTERS_HEATSHRINKFILE_H

#include <string>
#include <vector>

namespace mdf {
    enum class IsCompressedFileStatus : int {

    };

    bool isCompressedFile(std::string const& fileName);
    bool isCompressedFile(std::vector<uint8_t> const& data);

    bool decompressFile(std::string const& inputFile, std::string const& outputFile);
}

#endif //MDFSIMPLECONVERTERS_HEATSHRINKFILE_H
