#ifndef MDFSUPER_MDF_LIN_TRANSMISSIONERRORWRAPPER_H
#define MDFSUPER_MDF_LIN_TRANSMISSIONERRORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <LIN_TransmissionError.h>

struct MDF_LIN_TransmissionErrorWrapper : public Py::PythonExtension<MDF_LIN_TransmissionErrorWrapper> {
    MDF_LIN_TransmissionErrorWrapper(mdf::LIN_TransmissionError const &data);

    virtual ~MDF_LIN_TransmissionErrorWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Long ID;
};


#endif //MDFSUPER_MDF_LIN_TRANSMISSIONERRORWRAPPER_H
