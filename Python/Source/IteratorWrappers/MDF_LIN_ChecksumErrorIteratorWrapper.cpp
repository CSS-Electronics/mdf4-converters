#include "MDF_LIN_ChecksumErrorIteratorWrapper.h"

#include "../RecordWrappers/MDF_LIN_ChecksumErrorWrapper.h"

MDF_LIN_ChecksumErrorIteratorWrapper::MDF_LIN_ChecksumErrorIteratorWrapper(
    mdf::RecordIterator<mdf::LIN_ChecksumError const> const &parent) : it(parent) {
    //
}

MDF_LIN_ChecksumErrorIteratorWrapper::~MDF_LIN_ChecksumErrorIteratorWrapper() = default;

void MDF_LIN_ChecksumErrorIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.LIN_ChecksumError_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_LIN_ChecksumErrorIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_LIN_ChecksumErrorIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_LIN_ChecksumErrorIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_LIN_ChecksumErrorIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_LIN_ChecksumErrorWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
