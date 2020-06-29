#include "CallbackBuffer.h"

#include "PythonLogger.h"

namespace mdf::python {

    CallbackBuffer::CallbackBuffer(Py::Object obj, std::size_t cacheSize) :
        parent(obj),
        bufferOffset(0),
        cacheSize(cacheSize) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Constructing CallbackBuffer from " << obj << " with a cache size of " << cacheSize;

        // Check for compliance with the expected interface.
        do {
            BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Checking for compliance of required methods";
            if (!obj.hasAttr("read")) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::fatal) << "Python object does not fulfill requirements for being used in a CallbackBuffer, does it implement IFileInterface?";
                throw std::runtime_error("Python object does not fulfill interface requirements: Missing read method");
            }
            if (!obj.hasAttr("seek")) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::fatal) << "Python object does not fulfill requirements for being used in a CallbackBuffer, does it implement IFileInterface?";
                throw std::runtime_error("Python object does not fulfill interface requirements: Missing seek method");
            }

            auto const readMethod = obj.getAttr("read");
            auto const seekMethod = obj.getAttr("seek");

            // Attempt to find the required python module.
            PyObject* inspectModuleRaw = nullptr;
            try {
                inspectModuleRaw = PyImport_ImportModule("inspect");
                Py::ifPyErrorThrowCxxException();
            } catch(Py::BaseException &e) {
                e.clear();
                inspectModuleRaw = nullptr;
            }

            Py::Object inspectModule;

            if(inspectModuleRaw != nullptr) {
                inspectModule = Py::asObject(inspectModuleRaw);
            } else {
                throw std::runtime_error("Could not load \"inspect\" module");
            }

            auto const readSpec = inspectModule.callMemberFunction("getfullargspec", Py::TupleN(readMethod));
            auto const seekSpec = inspectModule.callMemberFunction("getfullargspec", Py::TupleN(seekMethod));

            // Result should be a tuple, with the first value as a list.
            if (!readSpec.isSequence() || !seekSpec.isSequence()) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::warning) << "Could not perform function introspection, getfullargspec";
                break;
            }

            auto const readSequence = static_cast<Py::Sequence>(readSpec);
            auto const seekSequence = static_cast<Py::Sequence>(seekSpec);

            if(readSequence.length() == 0 || seekSequence.length() == 0) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::warning) << "Could not perform function introspection, sequence";
                break;
            }

            auto const readArgs = readSequence[0];
            auto const seekArgs = seekSequence[0];

            if (!readArgs.isList() || !seekArgs.isList()) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::warning) << "Could not perform function introspection, list";
                break;
            }

            auto const readArgsCount = static_cast<Py::List>(readArgs).length();
            auto const seekArgsCount = static_cast<Py::List>(seekArgs).length();

            // Expect 2 arguments for read, and 2 for seek. Add an argument for each to handle the self/this object.
            if (readArgsCount != 3 || seekArgsCount != 3) {
                BOOST_LOG_SEV(pythonLogger::get(), severity_level::warning) << "Could not perform function introspection, list count";
                break;
            }

            BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Verified signature";
        } while(false);

        // Setup buffer.
        bufferStorage = std::move(std::unique_ptr<char[]>(new char[cacheSize], std::default_delete<char[]>()));
        buffer = bufferStorage.get();

        // Reset pointers.
        setg(buffer, buffer, buffer);
    }

    CallbackBuffer::traits_type::int_type CallbackBuffer::underflow() {
        // Create bytes buffer.
        PyObject* pythonBuffer = PyMemoryView_FromMemory(reinterpret_cast<char *>(eback()), cacheSize, 0x200);
        Py::Object wrappedBuffer = Py::asObject(pythonBuffer);
        Py::Long bufferSizeObj(static_cast<unsigned long>(cacheSize));

        // Call in python.
        Py::Object result = parent.callMemberFunction("read", Py::TupleN(wrappedBuffer, bufferSizeObj));
        Py::Long bytesRead(result);

        // Update pointers.
        setg(buffer, buffer, buffer + bytesRead);

        if(bytesRead == 0) {
            return traits_type::eof();
        } else {
            return traits_type::to_int_type( bufferStorage.get()[0]);
        }
    }

    CallbackBuffer::pos_type CallbackBuffer::seekoff(off_type offset, std::ios_base::seekdir direction, std::ios_base::openmode mode) {
        (void)mode;
        CallbackBuffer::pos_type result;

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

    CallbackBuffer::pos_type CallbackBuffer::seekpos(pos_type pos, std::ios_base::openmode mode) {
        return seekoff(pos, std::ios_base::beg, mode);
    }

    CallbackBuffer::pos_type CallbackBuffer::seekBeginning(off_type offset) {
        // Determine offset relative to current buffer.
        auto currentAvailable = egptr() - gptr();

        if((bufferOffset <= offset) && (offset < (bufferOffset + currentAvailable))) {
            auto difference = offset - bufferOffset;
            setg(eback(), buffer + difference, egptr());
        } else {
            // Reset.
            Py::Object result = parent.callMemberFunction("seek", Py::TupleN(Py::Long(offset), Py::Long(0)));
            Py::Long pos(result);

            bufferOffset = offset;
            setg(buffer, buffer, buffer);
        }

        return offset;
    }

    CallbackBuffer::pos_type CallbackBuffer::seekEnd(off_type offset) {
        Py::Object result = parent.callMemberFunction("seek", Py::TupleN(Py::Long(offset), Py::Long(2)));
        Py::Long pos(result);

        setg(buffer, buffer, buffer);
        return pos.as_unsigned_long_long();
    }

    CallbackBuffer::pos_type CallbackBuffer::seekCurrent(off_type offset) {
        if(offset == 0) {
            return currentPosition();
        }

        auto newAbsolute = currentPosition() + offset;

        // Determine offset relative to current buffer.
        auto currentAvailable = egptr() - gptr();

        if((bufferOffset <= newAbsolute) && (newAbsolute < (bufferOffset + currentAvailable))) {
            auto difference = newAbsolute - bufferOffset;
            setg(eback(), buffer + difference, egptr());
        } else {
            // Reset.
            Py::Object result = parent.callMemberFunction("seek", Py::TupleN(Py::Long(offset), Py::Long(1)));
            Py::Long pos(result);

            bufferOffset = newAbsolute;
            setg(buffer, buffer, buffer);
        }

        return newAbsolute;
    }

    CallbackBuffer::pos_type CallbackBuffer::currentPosition() {
        CallbackBuffer::pos_type result = bufferOffset;

        auto dif = gptr() - eback();
        if(dif < 0) {
            throw std::runtime_error("Buffer error");
        }

        result += dif;

        return result;
    }

    std::streamsize CallbackBuffer::xsgetn(char_type *s, std::streamsize n) {
        // If more data than is available is requested, redirect to the base.
        auto available = egptr() - gptr();
        std::streamsize bytesRead = 0;

        if(n > cacheSize) {
            // Cannot contain in a single read call, do a direct call.
            seekoff(currentPosition(), std::ios_base::beg, std::ios_base::in);
            PyObject* pythonBuffer = PyMemoryView_FromMemory(s, n, 0x200);
            Py::Object wrappedBuffer = Py::asObject(pythonBuffer);
            Py::Long bufferSizeObj(static_cast<unsigned long>(cacheSize));

            // Call in python.
            Py::Object result = parent.callMemberFunction("read", Py::TupleN(wrappedBuffer, bufferSizeObj));
            Py::Long bytesRead_(result);
            bytesRead = bytesRead_.as_unsigned_long_long();

            setg(buffer, buffer, buffer);
        } else if (n > available) {
            // Reset cache.
            auto pos = seekoff(currentPosition(), std::ios_base::beg, std::ios_base::in);
            underflow();

            std::copy_n(gptr(), n, s);
            setg(eback(), gptr() + n, egptr());
            if(egptr() - gptr() < 0) {
                throw std::runtime_error("Buffer error");
            }
            bytesRead = n;
        }
        else {
            std::copy_n(gptr(), n, s);
            setg(eback(), gptr() + n, egptr());
            if(egptr() - gptr() < 0) {
                throw std::runtime_error("Buffer error");
            }
            bytesRead = n;
        }

        return bytesRead;
    }

}
