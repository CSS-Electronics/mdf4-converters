#ifndef MDFSUPER_MDF_LIN_TRANSMISSIONERRORITERATORWRAPPER_H
#define MDFSUPER_MDF_LIN_TRANSMISSIONERRORITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <MdfFile.h>

struct MDF_LIN_TransmissionErrorIteratorWrapper : public Py::PythonExtension<MDF_LIN_TransmissionErrorIteratorWrapper> {
    MDF_LIN_TransmissionErrorIteratorWrapper(mdf::RecordIterator<mdf::LIN_TransmissionError const> const &parent);

    virtual ~MDF_LIN_TransmissionErrorIteratorWrapper();

    static void init_type(void);

    Py::Object getattro(const Py::String &name_);

    int setattro(const Py::String &name_, const Py::Object &value);

    Py::Object iter();

    PyObject *iternext();

protected:
    mdf::RecordIterator<mdf::LIN_TransmissionError const> it;
};


#endif //MDFSUPER_MDF_LIN_TRANSMISSIONERRORITERATORWRAPPER_H
