#ifndef MDFSUPER_MDF_LIN_RECEIVEERRORITERATORWRAPPER_H
#define MDFSUPER_MDF_LIN_RECEIVEERRORITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_LIN_ReceiveErrorIteratorWrapper : public Py::PythonExtension<MDF_LIN_ReceiveErrorIteratorWrapper> {
    MDF_LIN_ReceiveErrorIteratorWrapper(mdf::RecordIterator<mdf::LIN_ReceiveError const> const &parent);

    virtual ~MDF_LIN_ReceiveErrorIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::LIN_ReceiveError const> it;
};


#endif //MDFSUPER_MDF_LIN_RECEIVEERRORITERATORWRAPPER_H
