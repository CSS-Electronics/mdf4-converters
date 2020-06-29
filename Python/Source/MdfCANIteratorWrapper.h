#ifndef MDFSUPER_MDFCANITERATORWRAPPER_H
#define MDFSUPER_MDFCANITERATORWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include "MdfFileWrapper.h"
#include <MdfFile.h>

struct MdfCANIteratorWrapper : public Py::PythonExtension<MdfCANIteratorWrapper> {
  MdfCANIteratorWrapper(mdf::RecordIterator<mdf::CANRecord const> const& parent);
  virtual ~MdfCANIteratorWrapper();
  static void init_type(void);

  Py::Object getattro( const Py::String &name_ );
  int setattro( const Py::String &name_, const Py::Object &value );

  Py::Object iter();
  PyObject* iternext();

protected:
  mdf::RecordIterator<mdf::CANRecord const> it;
};


#endif //MDFSUPER_MDFCANITERATORWRAPPER_H
