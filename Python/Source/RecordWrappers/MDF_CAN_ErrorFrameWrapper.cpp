#include "MDF_CAN_ErrorFrameWrapper.h"

MDF_CAN_ErrorFrameWrapper::MDF_CAN_ErrorFrameWrapper(mdf::CAN_ErrorFrame const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ErrorType = static_cast<long>(data.ErrorType);

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ErrorType", ErrorType);
}

MDF_CAN_ErrorFrameWrapper::~MDF_CAN_ErrorFrameWrapper() = default;

void MDF_CAN_ErrorFrameWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_ErrorFrame");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_CAN_ErrorFrameWrapper::repr() {
    std::ostringstream s_out;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_CAN_ErrorFrameWrapper::getattro(const Py::String &s) {
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
