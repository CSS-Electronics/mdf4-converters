#include "AESGCMFile.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iterator>

#include "boost/endian/conversion.hpp"
#include "botan/pbkdf2.h"
#include "botan/cipher_mode.h"
#include "botan/aead.h"

namespace mdf {

    constexpr char magicHeader[12] = {'G', 'e', 'n', 'e', 'r', 'i', 'c', ' ', 'F', 'i', 'l', 'e'};
    constexpr char encryptedIdentifier = 0x11;
    constexpr char AESGCMIdentifier = 0x01;

    bool isEncryptedFile(std::string const& fileName) {
        // Open the file and compare the structure against the expected structure, and the supported capabilities by this library.
        std::ifstream fileStream(fileName, std::ifstream::in | std::ifstream::binary);

        // Read magic header and type information.
        char buffer[20];
        fileStream.read(buffer, sizeof(buffer));
        fileStream.close();

        if(fileStream.fail()) {
            return false;
        }

        // Check against expected header.
        if(!std::equal(magicHeader, magicHeader + sizeof(magicHeader), buffer)) {
            return false;
        }

        // Ensure it is compressed, and that the compression is supported by this library.
        if(buffer[14] != encryptedIdentifier) {
            return false;
        }

        if(buffer[15] != AESGCMIdentifier) {
            return false;
        }

        return true;
    }

    uint32_t getDeviceIdFromFile(std::string const& fileName) {
        uint32_t result = 0;

        // Open the file and compare the structure against the expected structure, and the supported capabilities by this library.
        std::ifstream fileStream(fileName, std::ifstream::in | std::ifstream::binary);

        // Read magic header and type information.
        char buffer[20];
        fileStream.read(buffer, sizeof(buffer));
        fileStream.close();

        result = boost::endian::load_big_u32(reinterpret_cast<uint8_t const*>(buffer) + 16);

        return result;
    }

    bool decryptFile(std::string const& inputFile, std::string const& outputFile, std::string const& password) {
        // Ensure the file is supported.
        if(!isEncryptedFile(inputFile)) {
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

        // Read AES GCM specific information.
        char bufferEncryptedHeader[32];
        inputStream.read(bufferEncryptedHeader, sizeof(bufferEncryptedHeader));

        if(inputStream.fail()) {
            return false;
        }

        std::array<uint8_t, 12> iv = { 0 };
        uint32_t iterations;
        std::array<uint8_t, 16> salt = { 0 };

        auto bufferPtr = reinterpret_cast<unsigned char const*>(bufferEncryptedHeader);

        std::copy(bufferPtr, bufferPtr + iv.size(), std::begin(iv));
        bufferPtr += iv.size();

        iterations = boost::endian::load_big_u32(bufferPtr);
        bufferPtr += sizeof(iterations);

        std::copy(bufferPtr, bufferPtr + salt.size(), std::begin(salt));

        // Generate a PBKDF2 handle.
        auto pbkdf2 = Botan::get_pbkdf("PBKDF2(SHA-256)");

        if(!pbkdf2) {
            return false;
        }

        // Use the supplied password and the read salt and iteration count to generate the AES key.
        std::array<uint8_t, 32> key = { 0 };

        auto keyBytes = pbkdf2->derive_key(key.max_size(), password, salt.data(), salt.size(), iterations);
        std::copy(keyBytes.begin(), keyBytes.end(), key.begin());

        // Initialize cipher for decryption.
        auto cipher = Botan::AEAD_Mode::create("AES-256/GCM", Botan::DECRYPTION);

        // Set key.
        cipher->set_key(key.data(), key.max_size());

        // Seek to the end to determine the file length.
        size_t const currentPosition = inputStream.tellg();
        inputStream.seekg(0, std::ios::end);
        size_t const fileSize = inputStream.tellg();

        // Read out GCM tag.
        inputStream.seekg(-16, std::ios::end);
        std::array<uint8_t, 16> tag = { 0 };
        inputStream.read(reinterpret_cast<char*>(&tag), sizeof(tag));

        // Seek to beginning to include the header as AEAD.
        inputStream.seekg(0, std::ios::beg);
        std::vector<uint8_t> aeadData(currentPosition);
        inputStream.read(reinterpret_cast<char*>(aeadData.data()), aeadData.capacity());

        cipher->set_associated_data(aeadData.data(), aeadData.capacity());

        // Start of decryption proper.
        cipher->start(iv.data(), iv.max_size());

        // Read file in chunks and feed it to the decrypter. First, determine how much data must be present in the last
        // block feed to the decrypter.
        size_t const minimumTrailingBlockLength = cipher->minimum_final_size();

        // Partition the remaining data into chunks.
        // Any data left over from the chunks is added to the trailing chunk.
        size_t const remainingEncryptedDataLength = fileSize - currentPosition - minimumTrailingBlockLength;
        size_t const blockLength = cipher->update_granularity();
        size_t const chunks = remainingEncryptedDataLength / cipher->update_granularity();
        size_t const lastChunkLength = remainingEncryptedDataLength % cipher->update_granularity() + minimumTrailingBlockLength;

        // Open the output file for writing.
        std::ofstream outputStream(outputFile, std::fstream::out | std::fstream::binary);
        std::ostream_iterator<uint8_t> outputIter(outputStream);

        // Create a buffer to read data into.
        Botan::secure_vector<uint8_t> dataBuffer(blockLength);

        // Read all the chunks.
        for(size_t i = 0; i < chunks; ++i) {
            // Read a chunk from the file.
            dataBuffer.clear();
            for(size_t n = 0; n < blockLength; ++n) {
                dataBuffer.emplace_back(inputStream.get());
            }

            // Decrypt in place.
            cipher->update(dataBuffer);

            // Write to output file.
            std::copy(dataBuffer.cbegin(), dataBuffer.cend(), outputIter);
        }

        // Handle the trailing/last chunk.
        dataBuffer.clear();
        for(size_t n = 0; n < lastChunkLength; ++n) {
            dataBuffer.emplace_back(inputStream.get());
        }

        // Decrypt in place.
        try {
            cipher->finish(dataBuffer);
        } catch(Botan::Invalid_Authentication_Tag &e) {
            return false;
        }

        // Write to output file.
        std::copy(dataBuffer.cbegin(), dataBuffer.cend(), outputIter);

        return true;
    }

}
