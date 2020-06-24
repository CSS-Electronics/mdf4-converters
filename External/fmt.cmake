include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "fmt")

set(fmt_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "fmt root path")
set(ENV{fmt_ROOT} ${fmt_ROOT})
find_package(fmt 6.2.1)

if(NOT fmt_FOUND)
  message("External dependencies: fmt not found, creating external target.")

  ExternalProject_Add (
    ${EXTERNAL_PROJECT_NAME}_builder

    PREFIX            ${EXTERNAL_PROJECT_NAME}
    URL               https://github.com/fmtlib/fmt/releases/download/6.2.1/fmt-6.2.1.zip
    URL_HASH          MD5=e66b07ca9fc0f88283a9bb294d3b7d06

    # Setup CMake arguments to use the same flags and install to the local path instead of the global path.
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DFMT_TEST=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON

    BUILD_ALWAYS      OFF
    INSTALL_DIR       ${EXTERNAL_PROJECT_INSTALL_DIR}
  )

  # Add to targets list.
  list(APPEND EXTERNAL_TARGETS ${EXTERNAL_PROJECT_NAME}_builder)
  set(EXTERNAL_TARGETS ${EXTERNAL_TARGETS} PARENT_SCOPE)
else()
  message("External dependencies: fmt found")
endif()
