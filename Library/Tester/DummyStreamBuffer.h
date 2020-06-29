#ifndef MDFSUPER_DUMMYSTREAMBUFFER_H
#define MDFSUPER_DUMMYSTREAMBUFFER_H

#include <memory>
#include <streambuf>

namespace mdf {

    struct DummyStreamBuffer : public std::streambuf {
        DummyStreamBuffer(std::shared_ptr<std::streambuf> parent, std::size_t cacheSize);

    protected:
        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;
        std::streamsize xsgetn(char_type *s, std::streamsize n) override;
    private:
        std::shared_ptr<std::streambuf> parent;
        std::size_t cacheSize;
        std::unique_ptr<char[]> bufferStorage;
        char* buffer;
        std::streampos bufferOffset;

        pos_type seekBeginning(off_type offset);
        pos_type seekEnd(off_type offset);
        pos_type seekCurrent(off_type offset);
        pos_type currentPosition();
    };

}

#endif //MDFSUPER_DUMMYSTREAMBUFFER_H
