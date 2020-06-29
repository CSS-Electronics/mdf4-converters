project(mdf2peak)

# Set project information.
set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to PEAK Trace format")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 7)

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
    Source/PEAK_CAN_Exporter_1_1.cpp
    Source/PEAK_CAN_Exporter_1_1.h
    Source/PEAK_CAN_Exporter_2_1.cpp
    Source/PEAK_CAN_Exporter_2_1.h
    Source/PEAK_CAN_Exporter.cpp
    Source/PEAK_CAN_Exporter.h
    Source/PEAK_Exporter.cpp
    Source/PEAK_Exporter.h
    Source/PEAK_TraceFormat.cpp
    Source/PEAK_TraceFormat.h
    Source/main.cpp
    )

include_directories(Include)
add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME}
    ${RUNTIME_LIBRARIES}
    MDFSimpleConverters_Shared
    fmt::fmt
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )

pack_executable(${PROGRAM_NAME})
