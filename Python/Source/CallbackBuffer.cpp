#include "CallbackBuffer.h"

#include "PythonLogger.h"

#include "boost/iostreams/read.hpp"
#include "boost/iostreams/seek.hpp"

namespace bio = boost::iostreams;

namespace mdf::python {

    CallbackBuffer::CallbackBuffer(Py::Object obj) : parent(obj) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Constructing CallbackBuffer from " << obj;

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

        endFound = false;

        buffer.clear();
        setg(buffer.data(), buffer.data(), buffer.data());
    }

    CallbackBuffer::traits_type::int_type CallbackBuffer::underflow() {
        int_type result;

        if(endFound) {
            result = traits_type::eof();
        } else {
            // Create bytes buffer.
            std::array<char_type, 1024> pollBuffer = { 0 };
            PyObject *pythonBuffer = PyMemoryView_FromMemory(pollBuffer.data(), pollBuffer.size(), 0x200);
            Py::Object wrappedBuffer = Py::asObject(pythonBuffer);
            Py::Long bufferSizeObj(static_cast<unsigned long>(pollBuffer.size()));

            // Call in python.
            Py::Object pythonResult = parent.callMemberFunction("read", Py::TupleN(wrappedBuffer, bufferSizeObj));
            Py::Long bytesRead(pythonResult);

            if (bytesRead == 0) {
                result = traits_type::eof();
                endFound = true;
            } else {
                std::copy(pollBuffer.begin(), pollBuffer.begin() + bytesRead, std::back_inserter(buffer));
                auto offset = gptr() - eback();
                setg(buffer.data(), buffer.data() + offset, buffer.data() + buffer.size());
                result = std::char_traits<char_type>::to_int_type(*gptr());
            }
        }

        return result;
    }

    CallbackBuffer::pos_type CallbackBuffer::seekoff(
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

                    while(!endFound && (targetAbsolute < buffer.size())) {
                        underflow();
                    }

                    if(targetAbsolute >= buffer.size()) {
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

    CallbackBuffer::pos_type CallbackBuffer::seekpos(pos_type offset, std::ios_base::openmode mode) {
        return seekoff(offset, std::ios::beg, mode);
    }

}
