#include "MdfFileWrapper.h"

#include <boost/predef.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "CallbackBuffer.h"
#include "IteratorWrappers/MDF_CAN_DataFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_CAN_ErrorFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_CAN_RemoteFrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_ChecksumErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_FrameIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_ReceiveErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_SyncErrorIteratorWrapper.h"
#include "IteratorWrappers/MDF_LIN_TransmissionErrorIteratorWrapper.h"
#include "PythonLogger.h"
#include "TypeSupport.h"

static Py::Object createNumpyArray(std::size_t size, std::string const& dtype);

template<typename T>
static T* getDataPointer(Py::Object column);

MdfFileWrapper::MdfFileWrapper(Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds) :
    Py::PythonClass<MdfFileWrapper>::PythonClass(self, args, kwds) {

    constexpr static int DEFAULT_CACHE_SIZE = 4096;

    // Try to extract the arguments from keywords first, argument list second.
    Py::Long cacheSize = Py::Long(DEFAULT_CACHE_SIZE);
    Py::Dict passwords = Py::Dict();
    Py::Object dataSource = Py::None();

    auto argsIter = args.begin();

    if(kwds.hasKey("data_source")) {
        Py::Object dataSourceRaw = kwds.getItem("data_source");

        if(dataSourceRaw.isNull() || dataSourceRaw.isNone()) {
            throw Py::ValueError("Invalid argument for the data source");
        } else {
            dataSource = dataSourceRaw;
        }
    } else {
        if (argsIter == args.end()) {
            throw Py::ValueError("Missing argument for the data source");
        } else {
            dataSource = *argsIter++;
        }
    }

    if(kwds.hasKey("passwords")) {
        Py::Object passwordsRaw = kwds.getItem("passwords");

        if(passwordsRaw.isNull() || passwordsRaw.isNone()) {
            passwords = Py::Dict();
        } else {
            passwords = passwordsRaw;
        }
    } else {
        if (argsIter == args.end()) {
            passwords = Py::Dict();
        } else {
            passwords = *argsIter++;
        }
    }

    if(kwds.hasKey("cache_size")) {
        Py::Object cacheSizeRaw = kwds.getItem("cache_size");

        if(cacheSizeRaw.isNull() || cacheSizeRaw.isNone() || !cacheSizeRaw.isNumeric()) {
            throw Py::ValueError("Invalid argument for cache_size");
        } else {
            cacheSize = Py::Long(cacheSizeRaw);
        }
    } else {
        if (argsIter == args.end()) {
            cacheSize = Py::Long(DEFAULT_CACHE_SIZE);
        } else {
            cacheSize = *argsIter++;
        }
    }

    // Determine if the input is a file/path, a handle, or a wrapper.
    MdfFileInputType inputType = getInputType(dataSource);

    Py::String inputPath;
    Py::Object handle;
    Py::Object wrapper;

    switch(inputType) {
        case MdfFileInputType::Interface:
            wrapper = dataSource;
            break;
        case MdfFileInputType::Path:
            // No need to convert, since the IO module can handle both strings and Path objects.
            // Fallthrough.
        case MdfFileInputType::String:
        {
            auto module = Py::Module("io");

            Py::Tuple openArgs = Py::TupleN(dataSource);

            Py::Dict openKeywords;
            openKeywords["mode"] = Py::String("rb");

            handle = module.callMemberFunction("open", openArgs, openKeywords);
        }
            // Fallthrough.
        case MdfFileInputType::FileLike:
        {
            if(handle.isNone() || handle.isNull()) {
                handle = dataSource;
            }

            // Create an interface wrapper.
            auto module = Py::Module("mdf_iter");
            auto dict = module.getDict();
            auto interface = dict.getItem("FileInterface");

            Py::Tuple interfaceArgs = Py::TupleN(handle);
            Py::Dict interfaceKeywords;

            wrapper = module.callMemberFunction("FileInterface", interfaceArgs, interfaceKeywords);
        }
        default:
            break;
    }

    std::unique_ptr<mdf::python::CallbackBuffer> stream;

    try {
        stream = std::make_unique<mdf::python::CallbackBuffer>(wrapper);
    } catch (std::exception &e) {
        throw Py::RuntimeError(e.what());
    }

    std::map<std::string, std::string> passwordMap;
    for(auto const& val: passwords) {
        // Validate entry.
        if(!val.first.isString() || !val.second.isString()) {
            continue;
        }

        Py::String keyValue = Py::String(val.first);
        Py::String valValue = Py::String(val.second);

        passwordMap.insert(std::make_pair(keyValue, valValue));
    }

    try {
        backingFile = mdf::MdfFile::Create(std::move(stream), passwordMap);
    } catch(std::exception &e) {
        // Re-throw in Python.
        throw Py::RuntimeError(e.what());
    }

    if(!backingFile) {
        throw Py::RuntimeError("Could not parse input data");
    }
}

MdfFileWrapper::~MdfFileWrapper() = default;

Py::Object MdfFileWrapper::GetCAN_DataFrameIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::CAN_DataFrame> iterator;

    try {
        iterator = backingFile->getCANIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a CAN_DataFrame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_CAN_DataFrameIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetCAN_DataFrameIterator)

Py::Object MdfFileWrapper::GetCAN_ErrorFrameIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::CAN_ErrorFrame> iterator;

    try {
        iterator = backingFile->getCANErrorFrameIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a CAN_DataFrame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_CAN_ErrorFrameIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetCAN_ErrorFrameIterator)

Py::Object MdfFileWrapper::GetCAN_RemoteFrameIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::CAN_RemoteFrame> iterator;

    try {
        iterator = backingFile->getCANRemoteFrameIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a CAN_DataFrame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_CAN_RemoteFrameIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetCAN_RemoteFrameIterator)

Py::Object MdfFileWrapper::GetLIN_ChecksumErrorIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_ChecksumError> iterator;

    try {
        iterator = backingFile->getLINChecksumErrorIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN_Frame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_LIN_ChecksumErrorIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetLIN_ChecksumErrorIterator)

Py::Object MdfFileWrapper::GetLIN_FrameIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_Frame> iterator;

    try {
        iterator = backingFile->getLINIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN_Frame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_LIN_FrameIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetLIN_FrameIterator)

Py::Object MdfFileWrapper::GetLIN_ReceiveErrorIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_ReceiveError> iterator;

    try {
        iterator = backingFile->getLINReceiveErrorIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN_Frame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_LIN_ReceiveErrorIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetLIN_ReceiveErrorIterator)

Py::Object MdfFileWrapper::GetLIN_SyncErrorIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_SyncError> iterator;

    try {
        iterator = backingFile->getLINSyncErrorIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN_Frame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_LIN_SyncErrorIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetLIN_SyncErrorIterator)

Py::Object MdfFileWrapper::GetLIN_TransmissionErrorIterator() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_TransmissionError> iterator;

    try {
        iterator = backingFile->getLINTransmissionErrorIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN_Frame iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    return Py::asObject(new MDF_LIN_TransmissionErrorIteratorWrapper(iterator));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetLIN_TransmissionErrorIterator)

Py::Object MdfFileWrapper::GetFirstMeasurementTimeStamp() {
    Py::Long result;

    std::chrono::nanoseconds value = backingFile->getFirstMeasurement();

    result = value.count();

    return static_cast<Py::Object>(result);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetFirstMeasurementTimeStamp)

Py::Object MdfFileWrapper::GetMetadata() {
    Py::Dict result;

    // Extract metadata from the file.
    mdf::MetadataMap metadata = backingFile->getMetadata();

    // Loop over the map, create a dict instead.
    for(auto const& entry: metadata) {
        mdf::MdfMetadataEntry const& entryRef = entry.second;

        Py::Dict values;
        values["description"] = Py::String(entryRef.description);
        values["name"] = Py::String(entryRef.name);
        values["read_only"] = Py::Boolean(entryRef.readOnly);
        values["unit"] = Py::String(entryRef.unit);
        values["value_raw"] = Py::String(entryRef.valueRaw);
        values["value_type"] = Py::String(entryRef.valueType);

        result[entry.first] = values;
    }

    return static_cast<Py::Object>(result);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetMetadata)

Py::Object MdfFileWrapper::GetDataFrame_CAN_DataFrame() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::CAN_DataFrame> iterator;

    try {
        iterator = backingFile->getCANIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a CAN iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    // Load required modules.
    Py::Module np("numpy");
    Py::Module pd("pandas");
    Py::Module datetime("datetime");

    // Get the number of CAN records.
    std::size_t numberOfRecords = backingFile->getFileInfo().CANMessages;

    // Create numpy arrays for all CAN columns.
    auto columnTimeStamp = createNumpyArray(numberOfRecords, "datetime64[ns]");
    auto columnBusChannel = createNumpyArray(numberOfRecords, "uint8");
    auto columnID = createNumpyArray(numberOfRecords, "uint32");
    auto columnIDE = createNumpyArray(numberOfRecords, "bool");
    auto columnDLC = createNumpyArray(numberOfRecords, "uint8");
    auto columnDataLength = createNumpyArray(numberOfRecords, "uint8");
    auto columnDir = createNumpyArray(numberOfRecords, "bool");
    auto columnBRS = createNumpyArray(numberOfRecords, "bool");
    auto columnESI = createNumpyArray(numberOfRecords, "bool");
    auto columnEDL = createNumpyArray(numberOfRecords, "bool");
    auto columnDataBytes = createNumpyArray(numberOfRecords, "object");

    // Access the raw data pointers for all CAN columns.
    auto columnTimeStamp_ptr = getDataPointer<uint64_t>(columnTimeStamp);
    auto columnBusChannel_ptr = getDataPointer<uint8_t>(columnBusChannel);
    auto columnID_ptr = getDataPointer<uint32_t>(columnID);
    auto columnIDE_ptr = getDataPointer<bool>(columnIDE);
    auto columnDLC_ptr = getDataPointer<uint8_t>(columnDLC);
    auto columnDataLength_ptr = getDataPointer<uint8_t>(columnDataLength);
    auto columnDir_ptr = getDataPointer<bool>(columnDir);
    auto columnBRS_ptr = getDataPointer<bool>(columnBRS);
    auto columnESI_ptr = getDataPointer<bool>(columnESI);
    auto columnEDL_ptr = getDataPointer<bool>(columnEDL);
    // NOTE: The raw pointer access to databytes is not present, since this an array of pointers to objects.

    // Iterate over all records, copying the data into the correct columns.
    Py::Tuple dataBytesArg;
    long long ctr(0);

    for(auto const& record: iterator) {
        // Raw data, copy directly.
        *columnTimeStamp_ptr++ = record.TimeStamp.count();
        *columnBusChannel_ptr++ = record.BusChannel;
        *columnID_ptr++ = record.ID;
        *columnIDE_ptr++ = record.IDE;
        *columnDLC_ptr++ = record.DLC;
        *columnDataLength_ptr++ = record.DataLength;
        *columnDir_ptr++ = record.Dir;
        *columnBRS_ptr++ = record.BRS;
        *columnESI_ptr++ = record.ESI;
        *columnEDL_ptr++ = record.EDL;

        // Databytes is vector like, copy as a list.
        Py::List dataBytes(record.DataBytes.size());
        for(int i = 0; i < record.DataBytes.size(); ++i) {
            dataBytes.setItem(i, Py::Long(record.DataBytes[i]));
        }

        //dataBytesArg = Py::TupleN(ctr++, dataBytes);
        dataBytesArg = Py::TupleN(Py::Long(ctr++), dataBytes);
        columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);

        // NOTE: Code for copying as bytes kept for reference.
        // auto ptr = reinterpret_cast<char const*>(record.DataBytes.data());
        // dataBytesArg = Py::TupleN(Py::Long(ctr++), Py::Bytes(ptr, record.DataBytes.size()));
        // columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);
    }

    // Create index from the timestamp column.
    Py::Dict indexArgs;
    auto const timezone = datetime.getAttr("timezone");
    auto const utc = timezone.getAttr("utc");
    indexArgs["tz"] = utc;
    Py::Object index = pd.callMemberFunction("DatetimeIndex", Py::TupleN(columnTimeStamp), indexArgs);
    index.setAttr("name", Py::String("TimeStamp"));

    // Create a pandas data frame with the index.
    Py::Dict dataFrameArgs;
    dataFrameArgs["index"] = index;
    Py::Object dataFrame = pd.callMemberFunction("DataFrame", Py::Tuple(), dataFrameArgs);

    // Merge the columns into the data frame.
    Py::Tuple insertArgs;
    int columnCounter = 0;

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("BusChannel"), columnBusChannel);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("ID"), columnID);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("IDE"), columnIDE);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DLC"), columnDLC);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataLength"), columnDataLength);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("Dir"), columnDir);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("EDL"), columnEDL);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("ESI"), columnESI);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("BRS"), columnBRS);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataBytes"), columnDataBytes);
    dataFrame.callMemberFunction("insert", insertArgs);

    return static_cast<Py::Object>(dataFrame);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetDataFrame_CAN_DataFrame)

Py::Object MdfFileWrapper::GetDataFrame_LIN_Frame() {
    // Extract an iterator to ensure the file is finalized and sorted.
    mdf::RecordIterator<const mdf::LIN_Frame> iterator;

    try {
        iterator = backingFile->getLINIterator();
    } catch (std::exception &e) {
        // Unexpected error, raise to Python.
        std::stringstream ss;

        ss << "An unexpected error occurred while obtaining a LIN iterator: ";
        ss << e.what() << std::endl;

        throw Py::RuntimeError(ss.str());
    }

    // Load required modules.
    Py::Module np("numpy");
    Py::Module pd("pandas");
    Py::Module datetime("datetime");

    // Get the number of LIN records.
    std::size_t numberOfRecords = backingFile->getFileInfo().LINMessages;

    // Create numpy arrays for all LIN columns.
    auto columnTimeStamp = createNumpyArray(numberOfRecords, "datetime64[ns]");
    auto columnBusChannel = createNumpyArray(numberOfRecords, "uint8");
    auto columnID = createNumpyArray(numberOfRecords, "uint32");
    auto columnRDBC = createNumpyArray(numberOfRecords, "uint8");
    auto columnDataLength = createNumpyArray(numberOfRecords, "uint8");
    auto columnDir = createNumpyArray(numberOfRecords, "bool");
    auto columnDataBytes = createNumpyArray(numberOfRecords, "object");

    // Access the raw data pointers for all LIN columns.
    auto columnTimeStamp_ptr = getDataPointer<uint64_t>(columnTimeStamp);
    auto columnBusChannel_ptr = getDataPointer<uint8_t>(columnBusChannel);
    auto columnID_ptr = getDataPointer<uint32_t>(columnID);
    auto columnRDBC_ptr = getDataPointer<uint8_t>(columnRDBC);
    auto columnDataLength_ptr = getDataPointer<uint8_t>(columnDataLength);
    auto columnDir_ptr = getDataPointer<bool>(columnDir);
    // NOTE: The raw pointer access to databytes is not present, since this an array of pointers to objects.

    // Iterate over all records, copying the data into the correct columns.
    Py::Tuple dataBytesArg;
    long long ctr(0);

    for(auto const& record: iterator) {
        // Raw data, copy directly.
        *columnTimeStamp_ptr++ = record.TimeStamp.count();
        *columnBusChannel_ptr++ = record.BusChannel;
        *columnID_ptr++ = record.ID;
        *columnRDBC_ptr++ = record.ReceivedDataByteCount;
        *columnDataLength_ptr++ = record.DataLength;
        *columnDir_ptr++ = record.Dir;

        // Databytes is vector like, copy as a list.
        Py::List dataBytes(record.DataBytes.size());
        for(int i = 0; i < record.DataBytes.size(); ++i) {
            dataBytes.setItem(i, Py::Long(record.DataBytes[i]));
        }

        dataBytesArg = Py::TupleN(Py::Long(ctr++), dataBytes);
        columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);

        // NOTE: Code for copying as bytes kept for reference.
        // auto ptr = reinterpret_cast<char const*>(record.DataBytes.data());
        // dataBytesArg = Py::TupleN(Py::Long(ctr++), Py::Bytes(ptr, record.DataBytes.size()));
        // columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);
    }

    // Create index from the timestamp column.
    Py::Dict indexArgs;
    auto const timezone = datetime.getAttr("timezone");
    auto const utc = timezone.getAttr("utc");
    indexArgs["tz"] = utc;
    Py::Object index = pd.callMemberFunction("DatetimeIndex", Py::TupleN(columnTimeStamp), indexArgs);
    index.setAttr("name", Py::String("TimeStamp"));

    // Create a pandas data frame with the index.
    Py::Dict dataFrameArgs;
    dataFrameArgs["index"] = index;
    Py::Object dataFrame = pd.callMemberFunction("DataFrame", Py::Tuple(), dataFrameArgs);

    // Merge the columns into the data frame.
    Py::Tuple insertArgs;
    int columnCounter = 0;

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("BusChannel"), columnBusChannel);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("ID"), columnID);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("ReceivedDataBytesCount"), columnRDBC);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataLength"), columnDataLength);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("Dir"), columnDir);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataBytes"), columnDataBytes);
    dataFrame.callMemberFunction("insert", insertArgs);

    return static_cast<Py::Object>(dataFrame);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetDataFrame_LIN_Frame)

Py::Object MdfFileWrapper::GetDataFrameNotAvailable() {
    std::cout << "Export to dataframe is not available. Ensure that numpy and pandas are installed." << std::endl;

    return Py::None();
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetDataFrameNotAvailable)

Py::Object MdfFileWrapper::Enter() {
    return self();
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, Enter)

Py::Object MdfFileWrapper::Exit(Py::Tuple args, Py::Dict keywords) {
    // Extract arguments.
    // exc_type, exc_value, traceback
    Py::Object exc_type;
    Py::Object exc_value;
    Py::Object traceback;

    // Extract from keywords first.
    if(keywords.hasKey("exc_type")) {
        exc_type = keywords.getItem("exc_type");
    }
    if(keywords.hasKey("exc_value")) {
        exc_value = keywords.getItem("exc_value");
    }
    if(keywords.hasKey("traceback")) {
        traceback = keywords.getItem("traceback");
    }

    switch(args.length()) {
        case 3:
            traceback = args[2];
            // Fallthrough.
        case 2:
            exc_value = args[1];
            // Fallthrough.
        case 1:
            exc_type = args[0];
            // Fallthrough.
        default:
            break;
    }

    return Py::None();
}

PYCXX_KEYWORDS_METHOD_DECL(MdfFileWrapper, Exit)

void MdfFileWrapper::init_type() {
    behaviors().name("mdf_iter.MdfFile");
    behaviors().doc("Read-only representation of a MDF file containing bus logging data");
    behaviors().supportGetattro();
    behaviors().supportSetattro();

    // Check if the requirements for reading out into pandas data frames are present.
    BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Checking for requirements for export to data frames";
    bool toDataFrameRequirementsPresent = true;

    try {
        auto const numpyIsPresent = PyImport_ImportModule("numpy");
        Py::ifPyErrorThrowCxxException();
    } catch(Py::BaseException &e) {
        e.clear();
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "numpy is not present";
        toDataFrameRequirementsPresent = false;
    }

    try {
        auto const pandasIsPresent = PyImport_ImportModule("pandas");
        Py::ifPyErrorThrowCxxException();
    } catch(Py::BaseException &e) {
        e.clear();
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "pandas is not present";
        toDataFrameRequirementsPresent = false;
    }

    try {
        auto const numpyIsPresent = PyImport_ImportModule("datetime");
        Py::ifPyErrorThrowCxxException();
    } catch(Py::BaseException &e) {
        e.clear();
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "datetime is not present";
        toDataFrameRequirementsPresent = false;
    }

    if(toDataFrameRequirementsPresent) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Enabling data frame export";

        PYCXX_ADD_NOARGS_METHOD(
            get_data_frame,
            GetDataFrame_CAN_DataFrame,
            "Get data frame over CAN records"
        );

        PYCXX_ADD_NOARGS_METHOD(
            get_data_frame_lin,
            GetDataFrame_LIN_Frame,
            "Get data frame over LIN records"
        );
    } else {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Data frame export not enabled, inserting dummy function";

        PYCXX_ADD_NOARGS_METHOD(
            get_data_frame,
            GetDataFrameNotAvailable,
            "Get data frame over CAN records"
        );
    };

    PYCXX_ADD_NOARGS_METHOD(
        get_first_measurement,
        GetFirstMeasurementTimeStamp,
        "Get the timestamp of the first measurement in the data"
        );

    PYCXX_ADD_NOARGS_METHOD(
        get_metadata,
        GetMetadata,
        "Get metadata of the file"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_can_iterator,
        GetCAN_DataFrameIterator,
        "Get iterator over CAN records"
        );

    PYCXX_ADD_NOARGS_METHOD(
        get_can_error_iterator,
        GetCAN_ErrorFrameIterator,
        "Get iterator over CAN records"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_can_remote_iterator,
        GetCAN_RemoteFrameIterator,
        "Get iterator over CAN records"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_lin_checksum_iterator,
        GetLIN_ChecksumErrorIterator,
        "Get iterator over LIN Checksum Errors"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_lin_iterator,
        GetLIN_FrameIterator,
        "Get iterator over LIN Frames"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_lin_receive_iterator,
        GetLIN_ReceiveErrorIterator,
        "Get iterator over LIN Receive Errors"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_lin_sync_iterator,
        GetLIN_SyncErrorIterator,
        "Get iterator over LIN Sync Errors"
    );

    PYCXX_ADD_NOARGS_METHOD(
        get_lin_transmission_iterator,
        GetLIN_TransmissionErrorIterator,
        "Get iterator over LIN Transmission Errors"
    );

    PYCXX_ADD_NOARGS_METHOD(
            __enter__,
            Enter,
            ""
    );

    PYCXX_ADD_KEYWORDS_METHOD(
            __exit__,
            Exit,
            ""
    );

    behaviors().readyType();
}

Py::Object MdfFileWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MdfFileWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

static Py::Object createNumpyArray(std::size_t size, std::string const& dtype) {
    Py::Module np("numpy");

    auto args = Py::TupleN(
        Py::String(dtype)
        );
    auto dataType = np.callMemberFunction("dtype", args);

    auto result = np.callMemberFunction("empty", Py::TupleN(Py::Long(static_cast<unsigned long>(size)), dataType));

    return result;
}

template<typename T>
static T* getDataPointer(Py::Object column) {
    // See https://stackoverflow.com/a/11266170
    Py::Dict columnID_arrayInterface = column.getAttr("__array_interface__");
    if(columnID_arrayInterface.isNull() || columnID_arrayInterface.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer");
    }

    // Extract the type information.
    Py::String columnID_arrayInterface_typestr = columnID_arrayInterface.getItem("typestr");
    if(columnID_arrayInterface_typestr.isNull() || columnID_arrayInterface_typestr.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer type information, the pointer is not set");
    }

    std::string const ptrType = columnID_arrayInterface_typestr.as_std_string();

    if(ptrType.length() < 3) {
        // Expect at least 3 characters with type information. Some types may have more characters, for instance
        // datetime64, which has a unit in square brackets. "datetime64[ns]" is represented as "<M8[ns]" on a
        // little-endian system.
        throw Py::RuntimeError("Could not get numpy pointer type information, type string must be at least 3 characters long");
    }

    char endian = ptrType[0];
    char dataType = ptrType[1];
    char byteCount = ptrType[2];

    // Check endian.
    if constexpr(BOOST_ENDIAN_LITTLE_BYTE) {
        if (endian == '>') {
            throw Py::RuntimeError("numpy array does not use native endian format");
        }
    } else {
        if (endian == '<') {
            throw Py::RuntimeError("numpy array does not use native endian format");
        }
    }

    // Check the type.
    if constexpr(std::is_same_v<T, bool>) {
        if (dataType != 'b') {
            throw Py::RuntimeError("numpy array does not match the expected type (boolean)");
        }
    } else if constexpr(std::is_floating_point_v<T>) {
        if (dataType != 'f') {
            throw Py::RuntimeError("numpy array does not match the expected type (floating point)");
        }
    } else if constexpr(std::is_integral_v<T>) {
        if constexpr(std::is_unsigned_v<T>) {
            if ((dataType != 'u') && (dataType != 'M')) {
                throw Py::RuntimeError("numpy array does not match the expected type (unsigned)");
            }
        } else {
            if (dataType != 'i') {
                throw Py::RuntimeError("numpy array does not match the expected type (signed)");
            }
        }
    }

    // Ensure the number of bytes matches.
    if constexpr(sizeof(T) == 1) {
        if (byteCount != '1') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 2) {
        if (byteCount != '2') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 4) {
        if (byteCount != '4') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 8) {
        if (byteCount != '8') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    }

    Py::Tuple columnID_arrayInterface_data = columnID_arrayInterface.getItem("data");
    if(columnID_arrayInterface_data.isNull() || columnID_arrayInterface_data.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer part 2");
    }

    Py::Long ptrAddress = static_cast<Py::Long>(columnID_arrayInterface_data.getItem(0));

    T* address = reinterpret_cast<T*>(ptrAddress.as_unsigned_long_long());

    return address;
}
