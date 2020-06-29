#include "Module.h"

#if defined( _WIN32 )
#define EXPORT_SYMBOL __declspec( dllexport )
#else
#define EXPORT_SYMBOL
#endif

extern "C" EXPORT_SYMBOL PyObject *PyInit_mdf_iter()
{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
  Py::InitialisePythonIndirectPy::Interface();
#endif

  static auto* module = new Module;
  return module->module().ptr();
}

extern "C" EXPORT_SYMBOL PyObject *PyInit_mdf_iter_d()
{
  return PyInit_mdf_iter();
}
