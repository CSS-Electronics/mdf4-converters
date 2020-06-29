#ifndef MDFSUPER_MDF_CAN_ERRORFRAMEITERATORWRAPPER_H
#define MDFSUPER_MDF_CAN_ERRORFRAMEITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_CAN_ErrorFrameIteratorWrapper : public Py::PythonExtension<MDF_CAN_ErrorFrameIteratorWrapper> {
    MDF_CAN_ErrorFrameIteratorWrapper(mdf::RecordIterator<mdf::CAN_ErrorFrame const> const &parent);

    virtual ~MDF_CAN_ErrorFrameIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::CAN_ErrorFrame const> it;
};


#endif //MDFSUPER_MDF_CAN_ERRORFRAMEITERATORWRAPPER_H
