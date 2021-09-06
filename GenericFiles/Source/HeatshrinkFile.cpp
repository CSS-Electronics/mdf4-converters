#include "HeatshrinkFile.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

#include "boost/endian/conversion.hpp"

extern "C" {
    #include "heatshrink_decoder.h"
}

namespace mdf {

    constexpr char magicHeader[12] = {'G', 'e', 'n', 'e', 'r', 'i', 'c', ' ', 'F', 'i', 'l', 'e'};
    constexpr char encryptedIdentifier = 0x22;
    constexpr char AESGCMIdentifier = 0x01;

    bool isCompressedFile(std::string const& fileName) {
        // Open the file and compare the structure against the expected structure, and the supported capabilities by this library.
        std::ifstream fileStream(fileName, std::ifstream::in | std::ifstream::binary);

        // Read magic header and type information.
        char buffer[20];
        fileStream.read(buffer, sizeof(buffer));
        fileStream.close();

        if(fileStream.fail()) {
            return false;
        }

        std::vector<uint8_t> data(buffer, buffer + sizeof(buffer));

        return isCompressedFile(data);
    }

    bool isCompressedFile(std::vector<uint8_t> const& data) {
        if(data.size() < 20) {
            return false;
        }

        // Check against expected header.
        if(!std::equal(magicHeader, magicHeader + sizeof(magicHeader), data.cbegin())) {
            return false;
        }

        // Ensure it is compressed, and that the compression is supported by this library.
        if(data[14] != encryptedIdentifier) {
            return false;
        }

        if(data[15] != AESGCMIdentifier) {
            return false;
        }

        return true;
    }

    bool decompressFile(std::string const& inputFile, std::string const& outputFile) {
        // Ensure the file is supported.
        if(!isCompressedFile(inputFile)) {
            return false;
        }

        // Open the file and compare the structure against the expected structure, and the supported capabilities by this library.
        std::ifstream inputStream(inputFile, std::ifstream::in | std::ifstream::binary);

        // Read magic header and type information.
        char bufferGenericHeader[20];
        inputStream.read(bufferGenericHeader, sizeof(bufferGenericHeader));

        if(inputStream.fail()) {
            return false;
        }

        // Check against expected header.
        if(!std::equal(magicHeader, magicHeader + sizeof(magicHeader), bufferGenericHeader)) {
            return false;
        }

        // Ensure it is compressed, and that the compression is supported by this library.
        if(bufferGenericHeader[14] != encryptedIdentifier) {
            return false;
        }

        if(bufferGenericHeader[15] != AESGCMIdentifier) {
            return false;
        }

        // Read heatshrink specific information.
        char bufferCompressionHeader[8];
        inputStream.read(bufferCompressionHeader, sizeof(bufferCompressionHeader));

        if(inputStream.fail()) {
            return false;
        }

        auto bufferPtr = reinterpret_cast<unsigned char const*>(bufferCompressionHeader);

        uint32_t lookaheadSize = boost::endian::load_big_u32(bufferPtr);
        uint32_t windowSize = boost::endian::load_big_u32(bufferPtr + sizeof(lookaheadSize));

        // Translate window to a power of 2.
        uint32_t windowSizeLog;
        switch(windowSize) {
            case 256:
                windowSizeLog = 8;
                break;
            case 512:
                windowSizeLog = 9;
                break;
            case 1024:
                windowSizeLog = 10;
                break;
            default:
                windowSizeLog = 0;
                break;
        }

        if(windowSizeLog == 0) {
            return false;
        }

        // Create a heatshrink decompressor with the required parameters.
        heatshrink_decoder* decoder = heatshrink_decoder_alloc(1024, windowSizeLog, lookaheadSize);

        // Decode into the output file.
        std::ofstream outputStream(outputFile, std::fstream::out | std::fstream::binary);
        std::vector<char> dataBuffer(1024);
        auto dataBufferPtr = reinterpret_cast<uint8_t*>(dataBuffer.data());

        while(!inputStream.eof() && !inputStream.fail()) {
            bool eof = inputStream.eof();
            // Read a block of data.
            inputStream.read(dataBuffer.data(), 1024);
            size_t bytesRead = inputStream.gcount();

            // Decode block.
            size_t bytesSunk;
            HSD_sink_res sinkResult;
            HSD_poll_res pollResult;

            do {
                sinkResult = heatshrink_decoder_sink(decoder, dataBufferPtr, bytesRead, &bytesSunk);

                size_t bytesPolled;
                do {
                    pollResult = heatshrink_decoder_poll(decoder, dataBufferPtr, dataBuffer.capacity(), &bytesPolled);

                    outputStream.write(dataBuffer.data(), bytesPolled);
                } while(pollResult == HSDR_POLL_MORE);
            } while(sinkResult != HSDR_SINK_OK);
        }

        HSD_finish_res finishResult = heatshrink_decoder_finish(decoder);;

        while(finishResult == HSDR_FINISH_MORE) {
            size_t bytesPolled;
            heatshrink_decoder_poll(decoder, dataBufferPtr, dataBuffer.capacity(), &bytesPolled);

            outputStream.write(dataBuffer.data(), bytesPolled);

            finishResult = heatshrink_decoder_finish(decoder);
        }

        return true;
    }

}
