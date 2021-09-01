#ifndef MDFSUPER_HEATSHRINKSTREAM_H
#define MDFSUPER_HEATSHRINKSTREAM_H

#include <limits>
#include <memory>
#include <streambuf>
#include <vector>

namespace mdf::stream {

    enum struct HeatshrinkLookahead : uint8_t {
        Size4 = 4,
        INVALID = std::numeric_limits<uint8_t>::max(),
    };

    enum struct HeatshrinkWindow : uint32_t {
        Size256 = 8,
        Size512 = 9,
        Size1024 = 10,
        INVALID = std::numeric_limits<uint32_t>::max(),
    };

    struct HeatshrinkStream : public std::streambuf {
        explicit HeatshrinkStream(
                std::unique_ptr<std::streambuf> parentStream,
                HeatshrinkLookahead lookahead,
                HeatshrinkWindow window
                );
        HeatshrinkStream(HeatshrinkStream const& other) = delete;
    protected:
        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;
    private:
        std::unique_ptr<std::streambuf> parentStream;
        HeatshrinkLookahead lookahead;
        HeatshrinkWindow window;

        // Offset related variables
        std::streampos parentInitialOffset;
        std::streampos parentCurrentPosition;
        std::streampos currentPosition;

        // Buffer
        std::vector<HeatshrinkStream::char_type> buffer;
        bool endFound;

        struct HeatshrinkDecoderWrapper;
        std::unique_ptr<HeatshrinkDecoderWrapper> heatshrinkDecoder;

        // Functions
        void resetDecoder();
        void poll();
    };

    std::unique_ptr<std::streambuf> applyHeatshrinkFilter(std::unique_ptr<std::streambuf> stream);

}

#endif //MDFSUPER_HEATSHRINKSTREAM_H
