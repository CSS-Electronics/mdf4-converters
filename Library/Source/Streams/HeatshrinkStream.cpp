#include "HeatshrinkStream.h"

#include <bit>

#include "boost/endian/conversion.hpp"
#include "boost/iostreams/read.hpp"
#include "boost/iostreams/seek.hpp"

extern "C" {
    #include "heatshrink_decoder.h"
}

namespace bio = boost::iostreams;

namespace mdf::stream {

    constexpr static std::array<char, 12> magicHeader = {'G', 'e', 'n', 'e', 'r', 'i', 'c', ' ', 'F', 'i', 'l', 'e'};
    constexpr static char COMPRESSED_IDENTIFIER = 0x22;
    constexpr static char HEATSHRINK_IDENTIFIER = 0x01;
    constexpr static size_t CACHE_SIZE = 1024 * 16;

    std::unique_ptr<std::streambuf> applyHeatshrinkFilter(std::unique_ptr<std::streambuf> stream) {
        auto startPosition = bio::seek(*stream, 0, std::ios::cur);
        bool resultFlag = false;

        // Check if this is a valid generic file.
        do {
            std::array<uint8_t, 28> header = { 0 };

            auto bytesRead = bio::read(*stream, reinterpret_cast<char*>(header.data()), header.size());
            if(bytesRead != header.size()) {
                break;
            }

            if( !std::equal(magicHeader.cbegin(), magicHeader.cend(), header.data()) ) {
                break;
            }

            if(header[14] != COMPRESSED_IDENTIFIER) {
                break;
            }
            if(header[15] != HEATSHRINK_IDENTIFIER) {
                break;
            }

            uint32_t lookaheadSizeRaw = boost::endian::load_big_u32(reinterpret_cast<const uint8_t*>(header.data() + 20));
            uint32_t windowSizeRaw = boost::endian::load_big_u32(reinterpret_cast<const uint8_t*>(header.data() + 24));

            if(!std::has_single_bit(windowSizeRaw)) {
                break;
            }

            auto logPower = std::countr_zero(windowSizeRaw);

            if(logPower == 0) {
                break;
            }

            HeatshrinkLookahead lookahead = HeatshrinkLookahead::INVALID;
            switch(lookaheadSizeRaw) {
                case static_cast<std::underlying_type_t<HeatshrinkLookahead>>(HeatshrinkLookahead::Size4):
                {
                    lookahead = static_cast<HeatshrinkLookahead>(lookaheadSizeRaw);
                    break;
                }
                default:
                {
                    break;
                }
            }

            if(lookahead == HeatshrinkLookahead::INVALID)
            {
                break;
            }

            HeatshrinkWindow windowSize = HeatshrinkWindow::INVALID;
            switch(logPower) {
                case static_cast<std::underlying_type_t<HeatshrinkWindow>>(HeatshrinkWindow::Size256):
                    // Fallthrough
                case static_cast<std::underlying_type_t<HeatshrinkWindow>>(HeatshrinkWindow::Size512):
                    // Fallthrough
                case static_cast<std::underlying_type_t<HeatshrinkWindow>>(HeatshrinkWindow::Size1024):
                {
                    windowSize = static_cast<HeatshrinkWindow>(logPower);
                    break;
                }
                default:
                {
                    break;
                }
            }

            if(windowSize == HeatshrinkWindow::INVALID)
            {
                break;
            }

            // Create wrapping stream.
            try {
                stream = std::make_unique<HeatshrinkStream>(std::move(stream), lookahead, windowSize);
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

    struct HeatshrinkStream::HeatshrinkDecoderWrapper {
        HeatshrinkDecoderWrapper(HeatshrinkLookahead lookahead, HeatshrinkWindow window);
        HeatshrinkDecoderWrapper(HeatshrinkDecoderWrapper const&) = delete;
        ~HeatshrinkDecoderWrapper();

        heatshrink_decoder* inner();
    private:
        HeatshrinkLookahead lookahead;
        HeatshrinkWindow window;
        heatshrink_decoder* wrapped;
    };

    HeatshrinkStream::HeatshrinkDecoderWrapper::HeatshrinkDecoderWrapper(
            HeatshrinkLookahead lookahead,
            HeatshrinkWindow window
    ) :
            lookahead(lookahead),
            window(window) {
        wrapped = heatshrink_decoder_alloc(CACHE_SIZE, static_cast<uint8_t>(window), static_cast<uint8_t>(lookahead));

        if(wrapped == nullptr) {
            throw std::runtime_error("Could not allocate memory for heatshrink decoder");
        }
    }

    heatshrink_decoder *HeatshrinkStream::HeatshrinkDecoderWrapper::inner() {
        return wrapped;
    }

    HeatshrinkStream::HeatshrinkDecoderWrapper::~HeatshrinkDecoderWrapper() {
        if(wrapped != nullptr) {
            heatshrink_decoder_free(wrapped);
            wrapped = nullptr;
        }
    }

    HeatshrinkStream::HeatshrinkStream(
            std::unique_ptr<std::streambuf> parentStream,
            HeatshrinkLookahead lookahead,
            HeatshrinkWindow window
            ) :
            parentStream(std::move(parentStream)),
            lookahead(lookahead),
            window(window)
            {
        // Take the current stream position to be the correct offset.
        parentInitialOffset = bio::seek(*this->parentStream, 0, std::ios::cur);
        parentCurrentPosition = parentInitialOffset;
        currentPosition = 0;
        endFound = false;

        buffer.clear();
        setg(buffer.data(), buffer.data(), buffer.data());

        // Create initial decoder.
        resetDecoder();
    }

    HeatshrinkStream::pos_type HeatshrinkStream::seekoff(
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

    HeatshrinkStream::pos_type HeatshrinkStream::seekpos(pos_type offset, std::ios_base::openmode mode) {
        return seekoff(offset, std::ios::beg, mode);
    }

    HeatshrinkStream::int_type HeatshrinkStream::underflow() {
        int_type result = std::char_traits<char_type>::eof();

        if(endFound) {
            result = std::char_traits<char_type>::eof();
        } else {
            // Sink the next block of data into the decoder, and read as many bytes as possible.
            std::array<char_type, CACHE_SIZE> nextBlock = {0};
            auto bytesRead = bio::read(*parentStream, nextBlock.data(), nextBlock.size());

            if (bytesRead == std::char_traits<char_type>::eof()) {
                HSD_finish_res finishRes;
                do {
                    finishRes = heatshrink_decoder_finish(heatshrinkDecoder->inner());
                    poll();
                } while (finishRes == HSDR_FINISH_MORE);

                if (egptr() - gptr() != 0) {
                    result = std::char_traits<char_type>::to_int_type(*gptr());
                }

                endFound = true;
            } else {
                // Sink all the data into the decoder.
                std::size_t bytesSunk = 0;
                HSD_sink_res sinkResult = heatshrink_decoder_sink(
                        heatshrinkDecoder->inner(),
                        reinterpret_cast<uint8_t *>(nextBlock.data()),
                        bytesRead,
                        &bytesSunk
                );

                // Extract all bytes from the decoder.
                poll();

                result = std::char_traits<char_type>::to_int_type(*gptr());
            }
        }

        return result;
    }

    void HeatshrinkStream::poll() {
        std::array<char_type, CACHE_SIZE> pollBuffer = {0 };
        HSD_poll_res pollResult = HSDR_POLL_ERROR_UNKNOWN;

        do {
            std::size_t bytesPolled = 0;
            pollResult = heatshrink_decoder_poll(
                    heatshrinkDecoder->inner(),
                    reinterpret_cast<uint8_t *>(pollBuffer.data()),
                    pollBuffer.size(),
                    &bytesPolled
            );

            std::copy(pollBuffer.begin(), pollBuffer.begin() + bytesPolled, std::back_inserter(buffer));
        } while(pollResult == HSDR_POLL_MORE);

        auto offset = gptr() - eback();
        setg(buffer.data(), buffer.data() + offset, buffer.data() + buffer.size());
    }

    void HeatshrinkStream::resetDecoder() {
        heatshrinkDecoder = std::make_unique<HeatshrinkDecoderWrapper>(lookahead, window);
    }
}
