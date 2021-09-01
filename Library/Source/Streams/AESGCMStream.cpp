#include "AESGCMStream.h"

#include <iomanip>
#include <optional>
#include <sstream>

#include "boost/endian/conversion.hpp"
#include "boost/iostreams/read.hpp"
#include "boost/iostreams/seek.hpp"
#include "botan/pbkdf2.h"
#include "botan/cipher_mode.h"
#include "botan/aead.h"

namespace bio = boost::iostreams;

namespace mdf::stream {

    constexpr static std::array<char, 12> magicHeader = {'G', 'e', 'n', 'e', 'r', 'i', 'c', ' ', 'F', 'i', 'l', 'e'};
    constexpr static char ENCRYPTED_IDENTIFIER = 0x11;
    constexpr static char AESGCM_IDENTIFIER = 0x01;

    std::unique_ptr<std::streambuf> applyAESGCMFilter(
            std::unique_ptr<std::streambuf> stream,
            std::map<std::string, std::string> const& passwords,
            bool checkTag
    ) {
        auto startPosition = bio::seek(*stream, 0, std::ios::cur);
        bool resultFlag = false;

        // Check if this is a valid generic file.
        do {
            std::array<uint8_t, 52> header = { 0 };

            auto bytesRead = bio::read(*stream, reinterpret_cast<char*>(header.data()), header.size());
            if(bytesRead != header.size()) {
                break;
            }

            if( !std::equal(magicHeader.cbegin(), magicHeader.cend(), header.data()) ) {
                break;
            }

            if(header[14] != ENCRYPTED_IDENTIFIER) {
                break;
            }
            if(header[15] != AESGCM_IDENTIFIER) {
                break;
            }

            // Extract device to use as key for the password lookup.
            uint32_t deviceId = boost::endian::load_big_u32(header.data() + 16);
            std::stringstream stringstream;
            stringstream << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << deviceId;
            std::string deviceIdKey(stringstream.str());

            std::optional<std::string> foundPassword;

            auto locator = passwords.find(deviceIdKey);
            if(locator != passwords.cend()) {
                foundPassword = std::string(locator->second);
            } else {
                locator = passwords.find("DEFAULT");
                if(locator != passwords.cend()) {
                    foundPassword = std::string(locator->second);
                }
            }

            if(foundPassword->empty()) {
                break;
            }

            // Extract iv, salt and iterations.
            std::array<uint8_t, 12> iv = { 0 };
            std::copy_n(header.data() + 20, iv.size(), std::begin(iv));

            uint32_t iterations = boost::endian::load_big_u32(header.data() + 32);

            std::array<uint8_t, 16> salt = { 0 };
            std::copy_n(header.data() + 36, salt.size(), std::begin(salt));

            // Create wrapping stream.
            try {
                stream = std::make_unique<AESGCMStream>(std::move(stream), iv, salt, foundPassword.value(), iterations, checkTag);
            } catch (std::runtime_error(&e)) {
                break;
            }

            resultFlag = true;
        } while( false );

        if(!resultFlag) {
            bio::seek(*stream, startPosition, std::ios::beg);
            return stream;
        }

        return stream;
    }

    AESGCMStream::AESGCMStream(
            std::unique_ptr<std::streambuf> parentStream_,
            std::array<uint8_t, 12> iv,
            std::array<uint8_t, 16> salt,
            std::string password,
            uint32_t iterations,
            bool checkTag
            ) :
            parentStream(std::move(parentStream_)),
            endFound(false) {
        // Generate a PBKDF2 handle.
        auto pbkdf2 = Botan::get_pbkdf("PBKDF2(SHA-256)");

        if (!pbkdf2) {
            throw std::runtime_error("Could not initialize decrypting library");
        }

        // Use the supplied password and the read salt and iteration count to generate the AES key.
        std::array<uint8_t, 32> key = {0};

        auto keyBytes = pbkdf2->derive_key(key.max_size(), password, salt.data(), salt.size(), iterations);
        std::copy(keyBytes.begin(), keyBytes.end(), key.begin());

        // Initialize cipher for decryption.
        cipher = Botan::AEAD_Mode::create("AES-256/GCM", Botan::DECRYPTION);

        // Set key.
        cipher->set_key(key.data(), key.max_size());

        // Seek to beginning to include the header as AEAD.
        currentPosition = bio::seek(*parentStream, 0, std::ios::cur);
        bio::seek(*parentStream, 0, std::ios::beg);
        std::vector<uint8_t> aeadData(currentPosition);
        auto bytesRead = bio::read(*parentStream, reinterpret_cast<char*>(aeadData.data()), currentPosition);

        if(bytesRead != currentPosition) {
            throw std::runtime_error("Could not read additional data to authenticate");
        }
        cipher->set_associated_data(aeadData.data(), bytesRead);

        // Determine file length.
        endLocation = bio::seek(*parentStream, 0, std::ios::end);
        dataEndLocation = endLocation;
        dataEndLocation -= cipher->tag_size();
        bio::seek(*parentStream, currentPosition, std::ios::beg);

        size_t const minimumTrailingBlockLength = cipher->minimum_final_size();
        size_t const remainingEncryptedDataLength = endLocation - currentPosition - minimumTrailingBlockLength;
        size_t const blockLength = cipher->update_granularity();
        size_t const chunks = remainingEncryptedDataLength / cipher->update_granularity();
        size_t const lastChunkLength = remainingEncryptedDataLength % cipher->update_granularity() + minimumTrailingBlockLength;

        readBuffer.reserve(cipher->update_granularity() + cipher->tag_size());

        // Start of decryption proper.
        cipher->start(iv.data(), iv.max_size());

        if(checkTag) {
            // Read to end.
            while(underflow() != std::char_traits<char_type>::eof()) {

            }
        }

        setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
    }

    AESGCMStream::int_type AESGCMStream::underflow() {
        int_type result;

        if(currentPosition == endLocation) {
            result = std::char_traits<char_type>::eof();
        } else {
            auto dataBytesAvailable = dataEndLocation - currentPosition;
            readBuffer.resize(128);

            if(dataBytesAvailable / cipher->update_granularity() != 0) {
                auto bytesRead = bio::read(*parentStream, reinterpret_cast<char_type*>(readBuffer.data()), cipher->update_granularity());
                currentPosition += bytesRead;
                readBuffer.resize(bytesRead);

                // Data decryption.
                if(bytesRead == std::char_traits<char_type>::eof()) {
                    throw std::runtime_error("End of file encountered before expected location");
                }
                auto bytesProcessed = cipher->process(readBuffer.data(), bytesRead);
                std::copy_n(readBuffer.cbegin(), bytesProcessed, std::back_inserter(buffer));

                auto offset = gptr() - eback();
                setg(buffer.data(), buffer.data() + offset, buffer.data() + buffer.size());
                result = std::char_traits<char_type>::to_int_type(*gptr());
            } else {
                // Tag decryption.
                auto bytesRead = bio::read(*parentStream, reinterpret_cast<char_type*>(readBuffer.data()), readBuffer.size());
                currentPosition += bytesRead;
                readBuffer.resize(bytesRead);

                if(bytesRead != dataBytesAvailable + cipher->tag_size()) {
                    throw std::runtime_error("Read too many bytes in last chunk");
                }

                Botan::secure_vector<uint8_t> lastBlock;
                std::copy_n(readBuffer.cbegin(), bytesRead, std::back_inserter(lastBlock));

                cipher->finish(lastBlock);
                std::copy(lastBlock.cbegin(), lastBlock.cend(), std::back_inserter(buffer));

                if(dataBytesAvailable == 0) {
                    result = std::char_traits<char_type>::eof();
                } else {
                    auto offset = gptr() - eback();
                    setg(buffer.data(), buffer.data() + offset, buffer.data() + buffer.size());
                    result = std::char_traits<char_type>::to_int_type(*gptr());
                }

                endFound = true;
            }
        }

        return result;
    }

    AESGCMStream::pos_type AESGCMStream::seekoff(
            off_type offset,
            std::ios_base::seekdir direction,
            std::ios_base::openmode mode
    ) {
        switch (direction) {
            case std::ios::beg: {
                if(offset < 0) {
                    throw std::invalid_argument("Cannot seek negative from the beginning");
                }

                while(!endFound && (offset >= buffer.size())) {
                    underflow();
                }

                if(offset < buffer.size()) {
                    setg(buffer.data(), buffer.data() + offset, buffer.data() + buffer.size());
                } else {
                    setg(buffer.data(), buffer.data() + buffer.size(), buffer.data() + buffer.size());
                }

                break;
            }
            case std::ios::cur: {
                if(offset != 0) {
                    auto targetAbsolute = gptr() - eback() + offset;

                    while(!endFound && (targetAbsolute >= buffer.size())) {
                        underflow();
                    }

                    if(targetAbsolute < buffer.size()) {
                        setg(buffer.data(), buffer.data() + targetAbsolute, buffer.data() + buffer.size());
                    } else {
                        setg(buffer.data(), buffer.data() + buffer.size(), buffer.data() + buffer.size());
                    }
                }

                break;
            }
            case std::ios::end: {
                if(offset > 0) {
                    throw std::invalid_argument("Cannot seek positive from the end");
                }

                // Read the parent stream to the end.
                int_type read;
                do {
                    read = underflow();
                } while( read != std::char_traits<char_type>::eof());

                // Seek relative to the end.
                setg(buffer.data(), buffer.data() + buffer.size() + offset, buffer.data() + buffer.size());

                break;
            }
            default: {
                throw std::invalid_argument("Unexpected seek direction");
            }
        }

        return gptr() - eback();
    }

    AESGCMStream::pos_type AESGCMStream::seekpos(pos_type offset, std::ios_base::openmode mode) {
        return seekoff(offset, std::ios::beg, mode);
    }

}
