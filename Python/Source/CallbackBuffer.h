#ifndef MDFSUPER_CALLBACKBUFFER_H
#define MDFSUPER_CALLBACKBUFFER_H

#include <istream>
#include <memory>

#include "CXX/Objects.hxx"

namespace mdf::python {

    struct CallbackBuffer : std::streambuf {
        explicit CallbackBuffer(Py::Object obj, std::size_t cacheSize = 4096);

        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;
        std::streamsize xsgetn(char_type *s, std::streamsize n) override;

    private:
        Py::Object parent;

        std::size_t cacheSize;
        std::unique_ptr<char[]> bufferStorage;
        char* buffer;
        std::streamoff bufferOffset;

        pos_type seekBeginning(off_type offset);
        pos_type seekEnd(off_type offset);
        pos_type seekCurrent(off_type offset);
        pos_type currentPosition();
    };

}

#endif //MDFSUPER_CALLBACKBUFFER_H
