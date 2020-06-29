#ifndef MDFSUPER_MDFCANRECORDWRAPPER_H
#define MDFSUPER_MDFCANRECORDWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <CANRecord.h>

struct MdfCANRecordWrapper : public Py::PythonExtension<MdfCANRecordWrapper> {
    MdfCANRecordWrapper(mdf::CANRecord const &data);

    virtual ~MdfCANRecordWrapper();

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


#endif //MDFSUPER_MDFCANRECORDWRAPPER_H
