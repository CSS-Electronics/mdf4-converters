#include "MDF_LIN_TransmissionErrorWrapper.h"

MDF_LIN_TransmissionErrorWrapper::MDF_LIN_TransmissionErrorWrapper(mdf::LIN_TransmissionError const &data) {
    // Copy over data.
    TimeStamp = data.TimeStamp.count();
    BusChannel = static_cast<long>(data.BusChannel);
    ID = static_cast<long>(data.ID);

    // Set members.
    dict.setItem("TimeStamp", TimeStamp);
    dict.setItem("BusChannel", BusChannel);
    dict.setItem("ID", ID);
}

MDF_LIN_TransmissionErrorWrapper::~MDF_LIN_TransmissionErrorWrapper() = default;

void MDF_LIN_TransmissionErrorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_TransmissionError");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportRepr();

    behaviors().readyType();
}

Py::Object MDF_LIN_TransmissionErrorWrapper::repr() {
    std::ostringstream s_out;

    return static_cast<Py::Object>(Py::String(s_out.str()));
}

Py::Object MDF_LIN_TransmissionErrorWrapper::getattro(const Py::String &s) {
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
