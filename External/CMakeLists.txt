# Empty list containing all external targets.
set(EXTERNAL_TARGETS)

# Setup external paths.
set(EXTERNAL_PROJECT_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/Root")
set(EXTERNAL_PROJECT_INSTALL_DIR_CMAKE ${CMAKE_CURRENT_BINARY_DIR}/Root/cmake)
set(EXTERNAL_PROJECT_INSTALL_DIR_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/Root/include)
set(EXTERNAL_PROJECT_INSTALL_DIR_LIB ${CMAKE_CURRENT_BINARY_DIR}/Root/lib)
set(EXTERNAL_PROJECT_INSTALL_DIR_LIB64 ${CMAKE_CURRENT_BINARY_DIR}/Root/lib64)

# Setup CMake module path.
list(APPEND CMAKE_MODULE_PATH "${EXTERNAL_PROJECT_INSTALL_DIR_CMAKE}")
list(APPEND CMAKE_MODULE_PATH "${EXTERNAL_PROJECT_INSTALL_DIR_LIB}/cmake")
list(APPEND CMAKE_MODULE_PATH "${EXTERNAL_PROJECT_INSTALL_DIR_LIB64}/cmake")

# Setup pkgconfig.
set(CMAKE_PREFIX_PATH "${EXTERNAL_PROJECT_INSTALL_DIR_LIB}/pkgconfig")
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH TRUE)
set(PKG_CONFIG_PATH "${EXTERNAL_PROJECT_INSTALL_DIR_LIB}/pkgconfig")
set(ENV{PKG_CONFIG_PATH} "${EXTERNAL_PROJECT_INSTALL_DIR_LIB}/pkgconfig")
find_package(PkgConfig REQUIRED)

# Set parallel build information.
include(ProcessorCount)
ProcessorCount(N_CORES)
if(NOT N_CORES EQUAL 0)
  set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} ${N_CORES})
endif()

include(Boost.cmake)
include(botan.cmake)
include(fmt.cmake)
include(heatshrink.cmake)
include(PyCXX.cmake)

# Add overall target which depend on all external dependencies.
list(LENGTH EXTERNAL_TARGETS EXTERNAL_TARGETS_LENGTH)
if(${EXTERNAL_TARGETS_LENGTH} GREATER 0)
  message("Found ${EXTERNAL_TARGETS_LENGTH} targets")

  add_custom_target(
    ExternalDependencies
    DEPENDS ${EXTERNAL_TARGETS}
  )
endif()