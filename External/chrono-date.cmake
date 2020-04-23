include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "tinyxml2")

set(tinyxml2_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "tinyxml2 root path")
set(ENV{tinyxml2_ROOT} ${tinyxml2_ROOT})
find_package(tinyxml2 QUIET)

message("External dependencies: Searching for ${EXTERNAL_PROJECT_NAME}")
if(NOT tinyxml2_FOUND)
  message("External dependencies: ${EXTERNAL_PROJECT_NAME} not found, creating external target")

  ExternalProject_Add (
    ${EXTERNAL_PROJECT_NAME}_builder

    PREFIX            ${EXTERNAL_PROJECT_NAME}
    URL               https://github.com/leethomason/tinyxml2/archive/7.1.0.tar.gz
    URL_HASH          MD5=8a02c592702dcc7a061932425d8f3087

    # Setup CMake arguments to use the same flags and install to the local path instead of the global path.
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

    BUILD_ALWAYS      OFF
    INSTALL_DIR       ${EXTERNAL_PROJECT_INSTALL_DIR}
  )

  # Add to targets list.
  list(APPEND EXTERNAL_TARGETS ${EXTERNAL_PROJECT_NAME}_builder)
  set(EXTERNAL_TARGETS ${EXTERNAL_TARGETS} PARENT_SCOPE)
else()
  message("External dependencies: ${EXTERNAL_PROJECT_NAME} found")
endif()
