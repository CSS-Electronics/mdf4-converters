#include "Module.h"
#include "MdfCANIteratorWrapper.h"
#include "MdfCANRecordWrapper.h"
#include "MdfFileWrapper.h"
#include "PythonLogger.h"

PYCXX_USER_EXCEPTION_STR_ARG(SimpleError)

Module::Module() : Py::ExtensionModule<Module>("mdf_iter") {
    // Map logging from boost to python.
    mdf::python::setupLogging();

    MdfFileWrapper::init_type();
    MdfCANIteratorWrapper::init_type();
    MdfCANRecordWrapper::init_type();

    initialize("");

    Py::Dict dict(moduleDictionary());
    Py::Object MdfFileType(MdfFileWrapper::type());
    dict["MdfFile"] = MdfFileType;

    SimpleError::init(*this);
}

Module::~Module() = default;
