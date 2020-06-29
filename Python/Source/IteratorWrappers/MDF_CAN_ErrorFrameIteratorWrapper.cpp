#include "MDF_CAN_ErrorFrameIteratorWrapper.h"

#include "../RecordWrappers/MDF_CAN_ErrorFrameWrapper.h"

MDF_CAN_ErrorFrameIteratorWrapper::MDF_CAN_ErrorFrameIteratorWrapper(
    mdf::RecordIterator<mdf::CAN_ErrorFrame const> const &parent) : it(parent) {
    //
}

MDF_CAN_ErrorFrameIteratorWrapper::~MDF_CAN_ErrorFrameIteratorWrapper() = default;

void MDF_CAN_ErrorFrameIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_ErrorFrame_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_CAN_ErrorFrameIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_CAN_ErrorFrameIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_CAN_ErrorFrameIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_CAN_ErrorFrameIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_CAN_ErrorFrameWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
