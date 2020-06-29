#include "MDF_LIN_ChecksumErrorWrapper.h"

MDF_LIN_ChecksumErrorWrapper::MDF_LIN_ChecksumErrorWrapper(mdf::LIN_ChecksumError const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
    dict.setItem("DataLength", DataLength);
    dict.setItem("Dir", Dir);
    dict.setItem("Checksum", Checksum);
    dict.setItem("ReceivedDataByteCount", ReceivedDataByteCount);
    dict.setItem("DataBytes", DataBytes);
}

MDF_LIN_ChecksumErrorWrapper::~MDF_LIN_ChecksumErrorWrapper() = default;

void MDF_LIN_ChecksumErrorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_ChecksumError");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_LIN_ChecksumErrorWrapper::repr() {
    std::ostringstream s_out;
    s_out << ID << " - " << DataBytes;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_LIN_ChecksumErrorWrapper::getattro(const Py::String &s) {
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
