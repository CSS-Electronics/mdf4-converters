#ifndef MDFSUPER_MODULE_H
#define MDFSUPER_MODULE_H

#ifdef Py_DEBUG
#undef Py_DEBUG
#endif

#ifdef _DEBUG
#undef _DEBUG
#define NDEBUG
#endif

#include "CXX/Extensions.hxx"
#include "CXX/Objects.hxx"

struct Module : public Py::ExtensionModule<Module> {
    Module();

    ~Module() override;
};

#endif //MDFSUPER_MODULE_H
