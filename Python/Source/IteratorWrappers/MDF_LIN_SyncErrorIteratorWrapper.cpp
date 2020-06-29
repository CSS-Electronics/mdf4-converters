#include "MDF_LIN_SyncErrorIteratorWrapper.h"

#include "../RecordWrappers/MDF_LIN_SyncErrorWrapper.h"

MDF_LIN_SyncErrorIteratorWrapper::MDF_LIN_SyncErrorIteratorWrapper(
    mdf::RecordIterator<mdf::LIN_SyncError const> const &parent) : it(parent) {
    //
}

MDF_LIN_SyncErrorIteratorWrapper::~MDF_LIN_SyncErrorIteratorWrapper() = default;

void MDF_LIN_SyncErrorIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_SyncError_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_LIN_SyncErrorIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_LIN_SyncErrorIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_LIN_SyncErrorIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_LIN_SyncErrorIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_LIN_SyncErrorWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
