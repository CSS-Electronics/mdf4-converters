#include "MDF_CAN_DataFrameIteratorWrapper.h"

#include "../RecordWrappers/MDF_CAN_DataFrameWrapper.h"

MDF_CAN_DataFrameIteratorWrapper::MDF_CAN_DataFrameIteratorWrapper(
    mdf::RecordIterator<mdf::CAN_DataFrame const> const &parent) : it(parent) {
    //
}

MDF_CAN_DataFrameIteratorWrapper::~MDF_CAN_DataFrameIteratorWrapper() = default;

void MDF_CAN_DataFrameIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_DataFrame_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_CAN_DataFrameIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_CAN_DataFrameIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_CAN_DataFrameIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_CAN_DataFrameIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_CAN_DataFrameWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
