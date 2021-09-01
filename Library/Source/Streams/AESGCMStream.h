#ifndef MDFSUPER_AESGCMSTREAM_H
#define MDFSUPER_AESGCMSTREAM_H

#include <array>
#include <limits>
#include <map>
#include <memory>
#include <streambuf>
#include <vector>

#include "botan/cipher_mode.h"
#include "botan/aead.h"

namespace mdf::stream {

    struct AESGCMStream : public std::streambuf {
        explicit AESGCMStream(
                std::unique_ptr<std::streambuf> parentStream,
                std::array<uint8_t, 12> iv,
                std::array<uint8_t, 16> salt,
                std::string password,
                uint32_t iterations,
                bool checkTag = false
        );
        AESGCMStream(AESGCMStream const& other) = delete;
    protected:
        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;
    private:
        std::unique_ptr<std::streambuf> parentStream;

        std::streampos currentPosition;
        std::streampos endLocation;
        std::streampos dataEndLocation;
        std::unique_ptr<Botan::AEAD_Mode> cipher;
        std::vector<uint8_t> readBuffer;

        // Buffer
        std::vector<AESGCMStream::char_type> buffer;
        bool endFound;
    };

    std::unique_ptr<std::streambuf> applyAESGCMFilter(
            std::unique_ptr<std::streambuf> stream,
            std::map<std::string, std::string> const& passwords,
            bool checkTag = false
            );

}

#endif //MDFSUPER_AESGCMSTREAM_H
