#include "TypeSupport.h"

MdfFileInputType getInputType(Py::Object inputObject) {
    MdfFileInputType result = MdfFileInputType::Other;

    do {
        if(isInputString(inputObject)) {
            result = MdfFileInputType::String;
            break;
        }

        if(isInputFileInterface(inputObject)) {
            result = MdfFileInputType::Interface;
            break;
        }

        if(isInputPath(inputObject)) {
            result = MdfFileInputType::Path;
            break;
        }

        if(isInputFileLike(inputObject)) {
            result = MdfFileInputType::FileLike;
            break;
        }
    } while(false);

    return result;
}

bool isInputString(Py::Object inputObject) {
    bool result = inputObject.isString();

    return result;
}

bool isInputPath(Py::Object inputObject) {
    auto module = Py::Module("pathlib");
    auto dict = module.getDict();
    auto interfaceObject = dict.getItem("Path");

    int isInstance = 0;

    try {
        isInstance = PyObject_IsInstance(inputObject.ptr(), interfaceObject.ptr());
        Py::ifPyErrorThrowCxxException();
    } catch (Py::BaseException &e) {
        e.clear();
        isInstance = 0;
    }

    bool result = (isInstance == 1);

    return result;
}

bool isInputFileInterface(Py::Object inputObject) {
    auto module = Py::Module("mdf_iter");
    auto dict = module.getDict();
    auto interfaceObject = dict.getItem("IFileInterface");

    int isInstance = 0;

    try {
        isInstance = PyObject_IsInstance(inputObject.ptr(), interfaceObject.ptr());
        Py::ifPyErrorThrowCxxException();
    } catch (Py::BaseException &e) {
        e.clear();
        isInstance = 0;
    }

    bool result = (isInstance == 1);

    return result;
}

bool isInputFileLike(Py::Object inputObject) {
    auto module = Py::Module("io");
    auto dict = module.getDict();
    auto interfaceObject = dict.getItem("IOBase");

    int isInstance = 0;

    try {
        isInstance = PyObject_IsInstance(inputObject.ptr(), interfaceObject.ptr());
        Py::ifPyErrorThrowCxxException();
    } catch (Py::BaseException &e) {
        e.clear();
        isInstance = 0;
    }

    bool result = (isInstance == 1);

    return result;
}
