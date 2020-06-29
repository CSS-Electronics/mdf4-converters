#include "CachingStreamBuffer.h"

#include <algorithm>

namespace mdf {

    CachingStreamBuffer::CachingStreamBuffer(std::shared_ptr<std::streambuf> parent, std::size_t cacheSize) :
    parent(parent), cacheSize(cacheSize) {
        bufferStorage = std::move(std::unique_ptr<char[]>(new char[cacheSize], std::default_delete<char[]>()));
        buffer = bufferStorage.get();

        // Request the current position of the underlying buffer, to mirror the state.
        bufferOffset = parent->pubseekoff(0, std::ios_base::cur);

        setg(buffer, buffer, buffer);
    }

    CachingStreamBuffer::int_type CachingStreamBuffer::underflow() {
        // Attempt to read from the underlying stream.
        std::streamsize bytesRead = parent->sgetn(buffer, cacheSize);

        setg(buffer, buffer, buffer + bytesRead);

        return buffer[0];
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::seekoff(off_type offset, std::ios_base::seekdir direction, std::ios_base::openmode mode) {
        (void)mode;
        CachingStreamBuffer::pos_type result;

        // Switch on the seek direction.
        switch (direction) {
            case std::ios_base::beg:
                result = seekBeginning(offset);
                break;
            case std::ios_base::cur:
                result = seekCurrent(offset);
                break;
            case std::ios_base::end:
                result = seekEnd(offset);
                break;
            default:
                result = 0;
                break;
        }

        return result;
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::seekpos(pos_type pos, std::ios_base::openmode mode) {
        return seekoff(pos, std::ios_base::beg, mode);
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::seekBeginning(off_type offset) {
        // Determine offset relative to current buffer.
        auto currentAvailable = egptr() - gptr();

        if((bufferOffset <= offset) && (offset < (bufferOffset + currentAvailable))) {
            auto difference = offset - bufferOffset;
            setg(eback(), buffer + difference, egptr());
        } else {
            // Reset.
            auto pos = parent->pubseekoff(offset, std::ios_base::beg);
            if(pos != offset) {
                pos = parent->pubseekoff(0, std::ios_base::beg);
            }

            bufferOffset = offset;
            setg(buffer, buffer, buffer);
        }

        return offset;
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::seekEnd(off_type offset) {
        auto res = parent->pubseekoff(offset, std::ios_base::end);
        setg(buffer, buffer, buffer);
        return res;
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::seekCurrent(off_type offset) {
        if(offset == 0) {
            return currentPosition();
        }

        auto res = parent->pubseekoff(offset, std::ios_base::cur);
        setg(buffer, buffer, buffer);
        return res;
    }

    CachingStreamBuffer::pos_type CachingStreamBuffer::currentPosition() {
        CachingStreamBuffer::pos_type result = bufferOffset;

        auto dif = gptr() - eback();
        if(dif < 0) {
            throw std::runtime_error("Buffer error 3");
        }

        result += dif;

        return result;
    }

    std::streamsize CachingStreamBuffer::xsgetn(char_type *s, std::streamsize n) {
        // If more data than is available is requested, redirect to the base.
        auto available = egptr() - gptr();
        std::streamsize bytesRead = 0;

        if(n > cacheSize) {
            // Cannot contain in a single read call, do a direct call.
            seekoff(currentPosition(), std::ios_base::beg, std::ios_base::in);
            bytesRead = parent->sgetn(s, n);

            // Update buffer pointers to unset.
            setg(buffer, buffer, buffer);
        } else if (n > available) {
            // Reset cache.
            auto pos = seekoff(currentPosition(), std::ios_base::beg, std::ios_base::in);
            underflow();

            // Determine how many of the requested bytes are available.
            available = egptr() - gptr();

            if(n > available) {
                n = available;
            }

            // Copy either the full set or the available bytes, and update pointers.
            std::copy_n(gptr(), n, s);
            setg(eback(), gptr() + n, egptr());

            bytesRead = n;
        }
        else {
            std::copy_n(gptr(), n, s);
            setg(eback(), gptr() + n, egptr());
            if(egptr() - gptr() < 0) {
                throw std::runtime_error("Buffer error 2");
            }
            bytesRead = n;
        }

        return bytesRead;
    }
}
