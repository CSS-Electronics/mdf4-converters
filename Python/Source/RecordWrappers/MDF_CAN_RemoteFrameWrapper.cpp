#include "MDF_CAN_RemoteFrameWrapper.h"

MDF_CAN_RemoteFrameWrapper::MDF_CAN_RemoteFrameWrapper(mdf::CAN_RemoteFrame const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);
    IDE = data.IDE;
    DLC = static_cast<long>(data.DLC);
    DataLength = static_cast<long>(data.DataLength);
    Dir = data.Dir;

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
    dict.setItem("IDE", IDE);
    dict.setItem("DLC", DLC);
    dict.setItem("DataLength", DataLength);
    dict.setItem("Dir", Dir);
    dict.setItem("DataBytes", DataBytes);
}

MDF_CAN_RemoteFrameWrapper::~MDF_CAN_RemoteFrameWrapper() = default;

void MDF_CAN_RemoteFrameWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_RemoteFrame");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_CAN_RemoteFrameWrapper::repr() {
    std::ostringstream s_out;
    s_out << ID << " - " << DataBytes;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_CAN_RemoteFrameWrapper::getattro(const Py::String &s) {
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
