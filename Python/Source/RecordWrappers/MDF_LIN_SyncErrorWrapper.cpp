#include "MDF_LIN_SyncErrorWrapper.h"

MDF_LIN_SyncErrorWrapper::MDF_LIN_SyncErrorWrapper(mdf::LIN_SyncError const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("Baudrate", Baudrate);
}

MDF_LIN_SyncErrorWrapper::~MDF_LIN_SyncErrorWrapper() = default;

void MDF_LIN_SyncErrorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_SyncError");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_LIN_SyncErrorWrapper::repr() {
    std::ostringstream s_out;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_LIN_SyncErrorWrapper::getattro(const Py::String &s) {
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
