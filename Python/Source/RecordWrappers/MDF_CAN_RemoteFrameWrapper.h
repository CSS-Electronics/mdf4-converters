#ifndef MDFSUPER_MDF_CAN_REMOTEFRAMEWRAPPER_H
#define MDFSUPER_MDF_CAN_REMOTEFRAMEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <CAN_RemoteFrame.h>

struct MDF_CAN_RemoteFrameWrapper : public Py::PythonExtension<MDF_CAN_RemoteFrameWrapper> {
    MDF_CAN_RemoteFrameWrapper(mdf::CAN_RemoteFrame const &data);

    virtual ~MDF_CAN_RemoteFrameWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Long ID;
    Py::Boolean IDE;
    Py::Long DLC;
    Py::Long DataLength;
    Py::Boolean Dir;
    Py::Bytes DataBytes;
};


#endif //MDFSUPER_MDF_CAN_REMOTEFRAMEWRAPPER_H
