# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if (NOT PyCXX_ROOT)
  set(PyCXX_ROOT "$ENV{PyCXX_ROOT}")
endif ()

if (NOT PyCXX_ROOT)
  find_path(_PyCXX_ROOT NAMES include/PyCXX_common.h)
else ()
  set(_PyCXX_ROOT ${PyCXX_ROOT})
endif ()

find_path(PyCXX_INCLUDE_DIR
  NAMES CXX/Version.hxx
  HINTS ${_PyCXX_ROOT}/include
  )

if (NOT ${PyCXX_INCLUDE_DIR} STREQUAL "PyCXX_INCLUDE_DIR-NOTFOUND")
  # Path to common file with configuration parameters.
  set(_PyCXX_H ${PyCXX_INCLUDE_DIR}/CXX/Version.hxx)

  # Function to extract version information from the header.
  function(_PyCXX_EXTRACT _PyCXX_VER_COMPONENT _PyCXX_VER_OUTPUT)
    set(CMAKE_MATCH_1 "0")
    set(_PyCXX_expr "^[ \\t]*#define[ \\t]+${_PyCXX_VER_COMPONENT}[ \\t]+([0-9]+)$")
    file(STRINGS "${_PyCXX_H}" _PyCXX_ver REGEX "${_PyCXX_expr}")
    string(REGEX MATCH "${_PyCXX_expr}" PyCXX_ver "${_PyCXX_ver}")
    set(${_PyCXX_VER_OUTPUT} "${CMAKE_MATCH_1}" PARENT_SCOPE)
  endfunction()

  # Extract version information.
  _PyCXX_EXTRACT("PYCXX_VERSION_MAJOR" PYCXX_VERSION_MAJOR)
  _PyCXX_EXTRACT("PYCXX_VERSION_MINOR" PYCXX_VERSION_MINOR)
  _PyCXX_EXTRACT("PYCXX_VERSION_PATCH" PYCXX_VERSION_PATCH)

  if (PyCXX_FIND_VERSION_COUNT GREATER 2)
    set(PyCXX_VERSION "${PYCXX_VERSION_MAJOR}.${PYCXX_VERSION_MINOR}.${PYCXX_VERSION_PATCH}")
  else ()
    set(PyCXX_VERSION "${PYCXX_VERSION_MAJOR}.${PYCXX_VERSION_MINOR}")
  endif ()

endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PyCXX
  FOUND_VAR
    PyCXX_FOUND
  REQUIRED_VARS
    PyCXX_INCLUDE_DIR
  VERSION_VAR
    PyCXX_VERSION
  )

if (PyCXX_FOUND)
  # Export variables.
  set(PyCXX_INCLUDE_DIRS ${PyCXX_INCLUDE_DIR} ${_PyCXX_ROOT}/src/PyCXX/)

  set(PyCXX_SOURCES
    ${_PyCXX_ROOT}/src/PyCXX/Src/cxx_exceptions.cxx
    ${_PyCXX_ROOT}/src/PyCXX/Src/cxx_extensions.cxx
    ${_PyCXX_ROOT}/src/PyCXX/Src/cxxextensions.c
    ${_PyCXX_ROOT}/src/PyCXX/Src/cxxsupport.cxx
    ${_PyCXX_ROOT}/src/PyCXX/Src/IndirectPythonInterface.cxx
    )

  # Create target if not already present.
  if (NOT TARGET PyCXX::PyCXX)
    add_library(PyCXX::PyCXX INTERFACE IMPORTED)
    set_target_properties(PyCXX::PyCXX PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${PyCXX_INCLUDE_DIRS}"
      )

    set_target_properties(PyCXX::PyCXX PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS Py_LIMITED_API=0x03050000
        )

    target_sources(PyCXX::PyCXX
      INTERFACE ${PyCXX_SOURCES}
      )
  endif ()

endif ()
