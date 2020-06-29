#ifndef MDFSUPER_MDF_CAN_ERRORFRAMEWRAPPER_H
#define MDFSUPER_MDF_CAN_ERRORFRAMEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <CAN_ErrorFrame.h>

struct MDF_CAN_ErrorFrameWrapper : public Py::PythonExtension<MDF_CAN_ErrorFrameWrapper> {
    MDF_CAN_ErrorFrameWrapper(mdf::CAN_ErrorFrame const &data);

    virtual ~MDF_CAN_ErrorFrameWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Long ErrorType;
};


#endif //MDFSUPER_MDF_CAN_ERRORFRAMEWRAPPER_H
