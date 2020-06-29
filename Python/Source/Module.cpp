#include "Module.h"

#include "IteratorWrappers/MDF_CAN_DataFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_CAN_ErrorFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_CAN_RemoteFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_ChecksumErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_FrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_ReceiveErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_SyncErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_TransmissionErrorIteratorWrapper.h"

#include "RecordWrappers/MDF_CAN_DataFrameWrapper.h"
#include "RecordWrappers/MDF_CAN_ErrorFrameWrapper.h"
#include "RecordWrappers/MDF_CAN_RemoteFrameWrapper.h"
#include "RecordWrappers/MDF_LIN_ChecksumErrorWrapper.h"
#include "RecordWrappers/MDF_LIN_FrameWrapper.h"
#include "RecordWrappers/MDF_LIN_ReceiveErrorWrapper.h"
#include "RecordWrappers/MDF_LIN_SyncErrorWrapper.h"
#include "RecordWrappers/MDF_LIN_TransmissionErrorWrapper.h"

#include "MdfFileWrapper.h"
#include "PythonLogger.h"

Module::Module() : Py::ExtensionModule<Module>("mdf_iter") {
    // Map logging from boost to python.
    mdf::python::setupLogging();

    MdfFileWrapper::init_type();

    MDF_CAN_DataFrameIteratorWrapper::init_type();
    MDF_CAN_DataFrameWrapper::init_type();

    MDF_CAN_ErrorFrameIteratorWrapper::init_type();
    MDF_CAN_ErrorFrameWrapper::init_type();

    MDF_CAN_RemoteFrameIteratorWrapper::init_type();
    MDF_CAN_RemoteFrameWrapper::init_type();

    MDF_LIN_ChecksumErrorIteratorWrapper::init_type();
    MDF_LIN_ChecksumErrorWrapper::init_type();

    MDF_LIN_FrameIteratorWrapper::init_type();
    MDF_LIN_FrameWrapper::init_type();

    MDF_LIN_ReceiveErrorIteratorWrapper::init_type();
    MDF_LIN_ReceiveErrorWrapper::init_type();

    MDF_LIN_SyncErrorIteratorWrapper::init_type();
    MDF_LIN_SyncErrorWrapper::init_type();

    MDF_LIN_TransmissionErrorIteratorWrapper::init_type();
    MDF_LIN_TransmissionErrorWrapper::init_type();

    initialize("");

    Py::Dict dict(moduleDictionary());
    Py::Object MdfFileType(MdfFileWrapper::type());
    dict["MdfFile"] = MdfFileType;
}

Module::~Module() = default;
