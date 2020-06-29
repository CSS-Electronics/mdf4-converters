#ifndef MDFSUPER_TYPESUPPORT_H
#define MDFSUPER_TYPESUPPORT_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

enum struct MdfFileInputType {
    String,
    Path,
    Interface,
    FileLike,
    Other,
};

MdfFileInputType getInputType(Py::Object inputObject);

bool isInputString(Py::Object inputObject);
bool isInputPath(Py::Object inputObject);
bool isInputFileInterface(Py::Object inputObject);
bool isInputFileLike(Py::Object inputObject);

#endif //MDFSUPER_TYPESUPPORT_H
