#ifndef MDFSUPER_MDF_LIN_FRAMEITERATORWRAPPER_H
#define MDFSUPER_MDF_LIN_FRAMEITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_LIN_FrameIteratorWrapper : public Py::PythonExtension<MDF_LIN_FrameIteratorWrapper> {
    MDF_LIN_FrameIteratorWrapper(mdf::RecordIterator<mdf::LIN_Frame const> const &parent);

    virtual ~MDF_LIN_FrameIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::LIN_Frame const> it;
};


#endif //MDFSUPER_MDF_LIN_FRAMEITERATORWRAPPER_H
