#ifndef MDFSUPER_MDF_LIN_CHEKSUMERRORITERATORWRAPPER_H
#define MDFSUPER_MDF_LIN_CHEKSUMERRORITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_LIN_ChecksumErrorIteratorWrapper : public Py::PythonExtension<MDF_LIN_ChecksumErrorIteratorWrapper> {
    MDF_LIN_ChecksumErrorIteratorWrapper(mdf::RecordIterator<mdf::LIN_ChecksumError const> const &parent);

    virtual ~MDF_LIN_ChecksumErrorIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::LIN_ChecksumError const> it;
};


#endif //MDFSUPER_MDF_LIN_CHEKSUMERRORITERATORWRAPPER_H
