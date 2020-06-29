#ifndef MDFSUPER_MDF_CAN_REMOTEFRAMEITERATORWRAPPER_H
#define MDFSUPER_MDF_CAN_REMOTEFRAMEITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_CAN_RemoteFrameIteratorWrapper : public Py::PythonExtension<MDF_CAN_RemoteFrameIteratorWrapper> {
    MDF_CAN_RemoteFrameIteratorWrapper(mdf::RecordIterator<mdf::CAN_RemoteFrame const> const &parent);

    virtual ~MDF_CAN_RemoteFrameIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::CAN_RemoteFrame const> it;
};


#endif //MDFSUPER_MDF_CAN_REMOTEFRAMEITERATORWRAPPER_H
