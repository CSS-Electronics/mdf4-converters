#include "MDF_LIN_FrameIteratorWrapper.h"

#include "../RecordWrappers/MDF_LIN_FrameWrapper.h"

MDF_LIN_FrameIteratorWrapper::MDF_LIN_FrameIteratorWrapper(
    mdf::RecordIterator<mdf::LIN_Frame const> const &parent) : it(parent) {
    //
}

MDF_LIN_FrameIteratorWrapper::~MDF_LIN_FrameIteratorWrapper() = default;

void MDF_LIN_FrameIteratorWrapper::init_type() {
    behaviors().name("mdf_iter.LINFrameIterator");
    behaviors().doc("");
    behaviors().supportIter();

    behaviors().readyType();
}

Py::Object MDF_LIN_FrameIteratorWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MDF_LIN_FrameIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

Py::Object MDF_LIN_FrameIteratorWrapper::iter() {
    return Py::Object(this, false);
}

PyObject *MDF_LIN_FrameIteratorWrapper::iternext() {
    if (it != it.cend()) {
        auto result = Py::asObject(new MDF_LIN_FrameWrapper(*it++));

        result.increment_reference_count();
        return result.ptr();
    } else {
        return nullptr;
    }
}
