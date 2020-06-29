#include "MDF_LIN_TransmissionErrorIteratorWrapper.h"

#include "../RecordWrappers/MDF_LIN_TransmissionErrorWrapper.h"

MDF_LIN_TransmissionErrorIteratorWrapper::MDF_LIN_TransmissionErrorIteratorWrapper(
    mdf::RecordIterator<mdf::LIN_TransmissionError const> const &parent) : it(parent) {
    //
}

MDF_LIN_TransmissionErrorIteratorWrapper::~MDF_LIN_TransmissionErrorIteratorWrapper() = default;

void MDF_LIN_TransmissionErrorIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_TransmissionError_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_LIN_TransmissionErrorIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_LIN_TransmissionErrorIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_LIN_TransmissionErrorIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_LIN_TransmissionErrorIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_LIN_TransmissionErrorWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
