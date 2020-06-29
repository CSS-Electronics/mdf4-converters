#ifndef MDFSUPER_MDF_CAN_DATAFRAMEITERATORWRAPPER_H
#define MDFSUPER_MDF_CAN_DATAFRAMEITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_CAN_DataFrameIteratorWrapper : public Py::PythonExtension<MDF_CAN_DataFrameIteratorWrapper> {
    MDF_CAN_DataFrameIteratorWrapper(mdf::RecordIterator<mdf::CAN_DataFrame const> const &parent);

    virtual ~MDF_CAN_DataFrameIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::CAN_DataFrame const> it;
};


#endif //MDFSUPER_MDF_CAN_DATAFRAMEITERATORWRAPPER_H
