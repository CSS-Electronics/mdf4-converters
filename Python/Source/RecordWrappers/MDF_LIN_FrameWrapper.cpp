#include "MDF_LIN_FrameWrapper.h"

MDF_LIN_FrameWrapper::MDF_LIN_FrameWrapper(mdf::LIN_Frame const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);
    DataLength = static_cast<long>(data.DataLength);
    Dir = data.Dir;
    ReceivedDataByteCount = static_cast<long>(data.ReceivedDataByteCount);
    DataBytes = Py::Bytes(reinterpret_cast<char const*>(data.DataBytes.data()), data.DataBytes.size());

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
    dict.setItem("DataLength", DataLength);
    dict.setItem("Dir", Dir);
    dict.setItem("ReceivedDataByteCount", ReceivedDataByteCount);
    dict.setItem("DataBytes", DataBytes);
}

MDF_LIN_FrameWrapper::~MDF_LIN_FrameWrapper() = default;

void MDF_LIN_FrameWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_Frame");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_LIN_FrameWrapper::repr() {
    std::ostringstream s_out;
    s_out << ID << " - " << DataBytes;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_LIN_FrameWrapper::getattro(const Py::String &s) {
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
