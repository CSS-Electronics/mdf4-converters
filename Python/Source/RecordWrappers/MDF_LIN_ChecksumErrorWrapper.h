#ifndef MDFSUPER_MDF_LIN_CHECKSUMERRORWRAPPER_H
#define MDFSUPER_MDF_LIN_CHECKSUMERRORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <LIN_ChecksumError.h>

struct MDF_LIN_ChecksumErrorWrapper : public Py::PythonExtension<MDF_LIN_ChecksumErrorWrapper> {
    MDF_LIN_ChecksumErrorWrapper(mdf::LIN_ChecksumError const &data);

    virtual ~MDF_LIN_ChecksumErrorWrapper();

    static void init_type(void);

    Py::Object repr() override;

    virtual Py::Object getattro(const Py::String& name) override;

    Py::Object pubattr;

private:
    Py::Dict dict;

    Py::Long TimeStamp;
    Py::Long BusChannel;
    Py::Long ID;
    Py::Long Checksum;
    Py::Boolean Dir;
    Py::Long ReceivedDataByteCount;
    Py::Long DataLength;
    Py::Bytes DataBytes;
};


#endif //MDFSUPER_MDF_LIN_FRAMEWRAPPER_H
