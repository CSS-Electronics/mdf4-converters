project(mdf2clx000)

# Set project information.
set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to CLX000 format")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

# Set required additional packages.
find_package(fmt REQUIRED)
find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp" REQUIRED)

# Embed information in the source code.
configure_file(
  Source/Version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Source/Version.h
  @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

set(SOURCE_FILES
    Source/CLX000_BaseExporter.cpp
    Source/CLX000_BaseExporter.h
    Source/CLX000_CAN_Exporter.cpp
    Source/CLX000_CAN_Exporter.h
    Source/CLX000_Configuration.h
    Source/CLX000_Exporter.cpp
    Source/CLX000_Exporter.h
    Source/CLX000_LIN_Exporter.cpp
    Source/CLX000_LIN_Exporter.h
    Source/main.cpp
    )

include_directories(Include)
add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME}
    ${RUNTIME_LIBRARIES}
    MDFSimpleConverters_Shared
    fmt::fmt
    )

# Setup additional files.
set(ADDITIONAL_RELEASE_FILES
    config.ini
    README.md
    )

set_property(TARGET ${PROGRAM_NAME} PROPERTY ADDITIONAL_FILES ${ADDITIONAL_RELEASE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )

pack_executable(${PROGRAM_NAME})
