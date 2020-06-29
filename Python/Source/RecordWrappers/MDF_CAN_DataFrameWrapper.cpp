#include "MDF_CAN_DataFrameWrapper.h"

MDF_CAN_DataFrameWrapper::MDF_CAN_DataFrameWrapper(mdf::CAN_DataFrame const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);
    IDE = data.IDE;
    DLC = static_cast<long>(data.DLC);
    DataLength = static_cast<long>(data.DataLength);
    Dir = data.Dir;
    EDL = data.EDL;
    BRS = data.BRS;
    DataBytes = Py::Bytes(reinterpret_cast<char const*>(data.DataBytes.data()), data.DataBytes.size());

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
    dict.setItem("IDE", IDE);
    dict.setItem("DLC", DLC);
    dict.setItem("DataLength", DataLength);
    dict.setItem("Dir", Dir);
    dict.setItem("EDL", EDL);
    dict.setItem("BRS", BRS);
    dict.setItem("DataBytes", DataBytes);
}

MDF_CAN_DataFrameWrapper::~MDF_CAN_DataFrameWrapper() = default;

void MDF_CAN_DataFrameWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_DataFrame");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_CAN_DataFrameWrapper::repr() {
    std::ostringstream s_out;
    s_out << ID << " - " << DataBytes;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_CAN_DataFrameWrapper::getattro(const Py::String &s) {
    std::string name = s.as_std_string("UTF-8");
    Py::Object result;

    if(dict.hasKey(name)) {
        return dict.getItem(name);
    } else if(name == "__dict__") {
        result = dict;
    } else {
        result = genericGetAttro(s);
    }

    return result;
}
