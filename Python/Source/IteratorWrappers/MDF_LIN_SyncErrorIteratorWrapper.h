#ifndef MDFSUPER_MDF_LIN_SYNCERRORITERATORWRAPPER_H
#define MDFSUPER_MDF_LIN_SYNCERRORITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_LIN_SyncErrorIteratorWrapper : public Py::PythonExtension<MDF_LIN_SyncErrorIteratorWrapper> {
    MDF_LIN_SyncErrorIteratorWrapper(mdf::RecordIterator<mdf::LIN_SyncError const> const &parent);

    virtual ~MDF_LIN_SyncErrorIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::LIN_SyncError const> it;
};


#endif //MDFSUPER_MDF_LIN_SYNCERRORITERATORWRAPPER_H
