#ifndef MDFSUPER_MDFFILEWRAPPER_H
#define MDFSUPER_MDFFILEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include "MdfFile.h"

/**
 * MdfFileWrapper is wrapping the MdfFile interface using the new style in PyCXX.
 * The new style interface was chosen since this object is going to be constructed from Python only.
 */
struct MdfFileWrapper : public Py::PythonClass<MdfFileWrapper> {
    MdfFileWrapper(Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds);

    ~MdfFileWrapper() override;

    static void init_type();

    Py::Object GetCAN_DataFrameIterator();

    Py::Object GetCAN_ErrorFrameIterator();

    Py::Object GetCAN_RemoteFrameIterator();

    Py::Object GetLIN_ChecksumErrorIterator();

    Py::Object GetLIN_FrameIterator();

    Py::Object GetLIN_ReceiveErrorIterator();

    Py::Object GetLIN_SyncErrorIterator();

    Py::Object GetLIN_TransmissionErrorIterator();

    Py::Object GetFirstMeasurementTimeStamp();

    Py::Object GetDataFrame_CAN_DataFrame();

    Py::Object GetDataFrame_LIN_Frame();

    Py::Object GetDataFrameNotAvailable();

    Py::Object GetMetadata();

    Py::Object Enter();

    Py::Object Exit(Py::Tuple args, Py::Dict keywords);

    Py::Object getattro(const Py::String &name_) override;

    int setattro(const Py::String &name_, const Py::Object &value) override;

protected:
    std::unique_ptr<mdf::MdfFile> backingFile;
};

#endif //MDFSUPER_MDFFILEWRAPPER_H
