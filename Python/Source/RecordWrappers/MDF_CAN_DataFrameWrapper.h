#ifndef MDFSUPER_MDF_CAN_DATAFRAMEWRAPPER_H
#define MDFSUPER_MDF_CAN_DATAFRAMEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <CAN_DataFrame.h>

struct MDF_CAN_DataFrameWrapper : public Py::PythonExtension<MDF_CAN_DataFrameWrapper> {
    MDF_CAN_DataFrameWrapper(mdf::CAN_DataFrame const &data);

    virtual ~MDF_CAN_DataFrameWrapper();

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
    Py::Boolean EDL;
    Py::Boolean BRS;
    Py::Bytes DataBytes;
};


#endif //MDFSUPER_MDF_CAN_DATAFRAMEWRAPPER_H
