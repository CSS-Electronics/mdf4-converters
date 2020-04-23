# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if(NOT Heatshrink_ROOT)
  set(Heatshrink_ROOT "$ENV{Heatshrink_ROOT}")
endif()

if(NOT Heatshrink_ROOT)
  find_path(_Heatshrink_ROOT NAMES include/heatshrink_common.h)
else()
  set(_Heatshrink_ROOT ${Heatshrink_ROOT})
endif()

message("Heatshrink root: ${_Heatshrink_ROOT}")
find_path(Heatshrink_INCLUDE_DIR
  NAMES heatshrink_common.h
  HINTS ${_Heatshrink_ROOT}/include
  )

if(NOT ${Heatshrink_INCLUDE_DIR} STREQUAL "Heatshrink_INCLUDE_DIR-NOTFOUND")

# Path to common file with configuration parameters.
set(_Heatshrink_H ${Heatshrink_INCLUDE_DIR}/heatshrink_common.h)

# Function to extract version information from the header.
function(_Heatshrink_EXTRACT _Heatshrink_VER_COMPONENT _Heatshrink_VER_OUTPUT)
  set(CMAKE_MATCH_1 "0")
  set(_Heatshrink_expr "^[ \\t]*#define[ \\t]+${_Heatshrink_VER_COMPONENT}[ \\t]+([0-9]+)$")
  file(STRINGS "${_Heatshrink_H}" _Heatshrink_ver REGEX "${_Heatshrink_expr}")
  string(REGEX MATCH "${_Heatshrink_expr}" Heatshrink_ver "${_Heatshrink_ver}")
  set(${_Heatshrink_VER_OUTPUT} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

# Extract version information.
_Heatshrink_EXTRACT("HEATSHRINK_VERSION_MAJOR" Heatshrink_VERSION_MAJOR)
_Heatshrink_EXTRACT("HEATSHRINK_VERSION_MINOR" Heatshrink_VERSION_MINOR)
_Heatshrink_EXTRACT("HEATSHRINK_VERSION_PATCH" Heatshrink_VERSION_PATCH)

if(Heatshrink_FIND_VERSION_COUNT GREATER 2)
  set(Heatshrink_VERSION "${Heatshrink_VERSION_MAJOR}.${Heatshrink_VERSION_MINOR}.${Heatshrink_VERSION_PATCH}")
else()
  set(Heatshrink_VERSION "${Heatshrink_VERSION_MAJOR}.${Heatshrink_VERSION_MINOR}")
endif()

if(NOT ${CMAKE_C_PLATFORM_ID} STREQUAL "Windows")
  find_library(Heatshrink_LIBRARY
    NAMES
    heatshrink
    HINTS
    ${_Heatshrink_ROOT}/lib
    ${_Heatshrink_ROOT}/lib/x86_64-linux-gnu
    )
else()
  find_library(Heatshrink_LIBRARY
    NAMES
    heatshrink
    HINTS
    ${_Heatshrink_ROOT}/lib
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Heatshrink
  FOUND_VAR
    Heatshrink_FOUND
  REQUIRED_VARS
    Heatshrink_INCLUDE_DIR
    Heatshrink_LIBRARY
  VERSION_VAR
    Heatshrink_VERSION
  )

if(Heatshrink_FOUND)
  # Export variables.
  set(Heatshrink_LIBRARIES ${Heatshrink_LIBRARY})
  set(Heatshrink_INCLUDE_DIRS ${Heatshrink_INCLUDE_DIR})

  # Create target if not already present.
  if(NOT TARGET Heatshrink::Heatshrink)
    add_library(Heatshrink::Heatshrink UNKNOWN IMPORTED)
    set_target_properties(Heatshrink::Heatshrink PROPERTIES
      IMPORTED_LOCATION "${Heatshrink_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${Heatshrink_INCLUDE_DIR}"
      )
  else()
    print_target_properties(Heatshrink::Heatshrink)
  endif()

endif()
endif()