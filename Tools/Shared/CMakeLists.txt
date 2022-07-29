project(MDFSimpleConverters_Shared)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS filesystem program_options log REQUIRED)
find_package(Heatshrink 0.4.1 REQUIRED)

set(MDFSimpleConverters_Shared_VERSION_MAJOR 1)
set(MDFSimpleConverters_Shared_VERSION_MINOR 4)
set(MDFSimpleConverters_Shared_VERSION_PATCH 0)

find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp" REQUIRED)

# Embed information in the source code.
configure_file(
    Source/ProjectInformation.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProjectInformation.h
    @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

set(INCLUDE_FILES
    Include/CommonOptions.h
    Include/ConverterInterface.h
    Include/ExecutableEntryPoint.h
    Include/GenericRecordExporter.h
    Include/ParsedFileInfo.h
    Include/ParseOptionStatus.h
    Include/PasswordStorage.h
    Include/ProgressIndicator.h
    Include/TimeConverter.h
    )

set(SOURCE_FILES
    Source/ConverterInterface.cpp
    Source/ExecutableEntryPoint.cpp
    Source/ExecutableInterface.cpp
    Source/ParsedFileInfo.cpp
    Source/ParseOptionStatus.cpp
    Source/PasswordStorage.cpp
    Source/ProgressIndicator.cpp
    Source/StatusCode.cpp
    Source/StatusCode.h
    Source/TimeConverter.cpp
    )

include_directories(Include)

# Create static library.
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}
    Heatshrink::Heatshrink
    Boost::headers
    MDF_BusLogging_Library
    GenericFiles
    Boost::log
    Boost::program_options
    ${CMAKE_DL_LIBS}
    )

# Setup custom include paths.
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
target_include_directories(${PROJECT_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
