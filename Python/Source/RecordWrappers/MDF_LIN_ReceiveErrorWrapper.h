#ifndef MDFSUPER_MDF_LIN_RECEIVEERRORWRAPPER_H
#define MDFSUPER_MDF_LIN_RECEIVEERRORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <LIN_ReceiveError.h>

struct MDF_LIN_ReceiveErrorWrapper : public Py::PythonExtension<MDF_LIN_ReceiveErrorWrapper> {
    MDF_LIN_ReceiveErrorWrapper(mdf::LIN_ReceiveError const &data);

    virtual ~MDF_LIN_ReceiveErrorWrapper();

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


#endif //MDFSUPER_MDF_LIN_RECEIVEERRORWRAPPER_H
