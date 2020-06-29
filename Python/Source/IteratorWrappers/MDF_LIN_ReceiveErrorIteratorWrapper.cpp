#include "MDF_LIN_ReceiveErrorIteratorWrapper.h"

#include "../RecordWrappers/MDF_LIN_ReceiveErrorWrapper.h"

MDF_LIN_ReceiveErrorIteratorWrapper::MDF_LIN_ReceiveErrorIteratorWrapper(
    mdf::RecordIterator<mdf::LIN_ReceiveError const> const &parent) : it(parent) {
    //
}

MDF_LIN_ReceiveErrorIteratorWrapper::~MDF_LIN_ReceiveErrorIteratorWrapper() = default;

void MDF_LIN_ReceiveErrorIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_ReceiveError_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_LIN_ReceiveErrorIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_LIN_ReceiveErrorIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_LIN_ReceiveErrorIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_LIN_ReceiveErrorIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_LIN_ReceiveErrorWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
