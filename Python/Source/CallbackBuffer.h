#ifndef MDFSUPER_CALLBACKBUFFER_H
#define MDFSUPER_CALLBACKBUFFER_H

#include <istream>
#include <memory>
#include <vector>

#include "CXX/Objects.hxx"

namespace mdf::python {

    struct CallbackBuffer : public std::streambuf {
        explicit CallbackBuffer(Py::Object obj);

        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;
    private:
        Py::Object parent;

        std::streampos currentPositionParent;
        std::vector<CallbackBuffer::char_type> buffer;
        bool endFound;
    };

}

#endif //MDFSUPER_CALLBACKBUFFER_H
