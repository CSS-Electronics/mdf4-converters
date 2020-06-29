#ifndef MDFSUPER_MDF_LIN_SYNCERRORWRAPPER_H
#define MDFSUPER_MDF_LIN_SYNCERRORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <LIN_SyncError.h>

struct MDF_LIN_SyncErrorWrapper : public Py::PythonExtension<MDF_LIN_SyncErrorWrapper> {
    MDF_LIN_SyncErrorWrapper(mdf::LIN_SyncError const &data);

    virtual ~MDF_LIN_SyncErrorWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Float Baudrate;
};


#endif //MDFSUPER_MDF_LIN_SYNCERRORWRAPPER_H
