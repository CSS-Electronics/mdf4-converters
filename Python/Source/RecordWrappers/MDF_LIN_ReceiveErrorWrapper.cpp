#include "MDF_LIN_ReceiveErrorWrapper.h"

MDF_LIN_ReceiveErrorWrapper::MDF_LIN_ReceiveErrorWrapper(mdf::LIN_ReceiveError const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
}

MDF_LIN_ReceiveErrorWrapper::~MDF_LIN_ReceiveErrorWrapper() = default;

void MDF_LIN_ReceiveErrorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_ReceiveError");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_LIN_ReceiveErrorWrapper::repr() {
    std::ostringstream s_out;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_LIN_ReceiveErrorWrapper::getattro(const Py::String &s) {
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
