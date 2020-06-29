#include "MdfCANIteratorWrapper.h"
#include "MdfCANRecordWrapper.h"

MdfCANIteratorWrapper::MdfCANIteratorWrapper(mdf::RecordIterator<mdf::CANRecord const> const& parent) : it(parent) {
  //
}

MdfCANIteratorWrapper::~MdfCANIteratorWrapper() = default;

void MdfCANIteratorWrapper::init_type() {
  behaviors().name( "mdf_iter.CANIterator" );
  behaviors().doc( "" );
  behaviors().supportIter();

  behaviors().readyType();
}

Py::Object MdfCANIteratorWrapper::getattro(const Py::String &name_) {
  return genericGetAttro( name_ );
}

int MdfCANIteratorWrapper::setattro(const Py::String &name_, const Py::Object &value ) {
  return genericSetAttro( name_, value );
}

Py::Object MdfCANIteratorWrapper::iter() {
  return Py::Object(this, false);
}

PyObject* MdfCANIteratorWrapper::iternext() {
  if(it != it.cend()) {
    auto result = Py::asObject(new MdfCANRecordWrapper(*it++));

    result.increment_reference_count();
    return result.ptr();
  } else {
    return nullptr;
  }
}
