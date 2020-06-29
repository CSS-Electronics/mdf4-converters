#include "MDF_CAN_RemoteFrameIteratorWrapper.h"

#include "../RecordWrappers/MDF_CAN_RemoteFrameWrapper.h"

MDF_CAN_RemoteFrameIteratorWrapper::MDF_CAN_RemoteFrameIteratorWrapper(
    mdf::RecordIterator<mdf::CAN_RemoteFrame const> const &parent) : it(parent) {
    //
}

MDF_CAN_RemoteFrameIteratorWrapper::~MDF_CAN_RemoteFrameIteratorWrapper() = default;

void MDF_CAN_RemoteFrameIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.CAN_RemoteFrame_Iterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_CAN_RemoteFrameIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_CAN_RemoteFrameIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_CAN_RemoteFrameIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_CAN_RemoteFrameIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_CAN_RemoteFrameWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
