#ifndef MDFSUPER_MDF_LIN_FRAMEWRAPPER_H
#define MDFSUPER_MDF_LIN_FRAMEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <LIN_Frame.h>

struct MDF_LIN_FrameWrapper : public Py::PythonExtension<MDF_LIN_FrameWrapper> {
    MDF_LIN_FrameWrapper(mdf::LIN_Frame const &data);

    virtual ~MDF_LIN_FrameWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Long ID;
    Py::Boolean Dir;
    Py::Long ReceivedDataByteCount;
    Py::Long DataLength;
    Py::Bytes DataBytes;
};


#endif //MDFSUPER_MDF_LIN_FRAMEWRAPPER_H
