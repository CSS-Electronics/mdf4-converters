# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if(NOT AppImage_ROOT)
  set(AppImage_ROOT "$ENV{AppImage_ROOT}")
endif()

if(NOT AppImage_ROOT)
  find_path(_AppImage_ROOT
      NAMES opt/linuxdeploy-x86_64.AppImage
      )
else()
  set(_AppImage_ROOT ${AppImage_ROOT})
endif()

find_program(AppImage_EXECUTABLE
  NAMES opt/linuxdeploy-x86_64.AppImage
  HINTS ${_AppImage_ROOT}
  )

if(NOT ${AppImage_EXECUTABLE} STREQUAL "AppImage_EXECUTABLE-NOTFOUND")

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AppImage
  FOUND_VAR
    AppImage_FOUND
  REQUIRED_VARS
    AppImage_EXECUTABLE
  )
