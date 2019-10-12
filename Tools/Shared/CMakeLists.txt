project(ToolsBase CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(SOURCE_FILES
    Source/ConverterInterface.cpp
    Source/ExecutableInterface.cpp
    Source/ExecutableInterface.h
    Source/ExecutableEntryPoint.cpp
    Source/ParseOptionStatus.cpp
    Source/TimeConverter.cpp
    )

set(INTERFACE_FILES
    Include/CommonOptions.h
    Include/ConverterInterface.h
    Include/ExecutableEntryPoint.h
    Include/GenericRecordExporter.h
    Include/ParseOptionStatus.h
    Include/TimeConverter.h
    )

# Embed version information in the source code.
configure_file(Source/VersionInformation.h.in VersionInformation.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(Include)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INTERFACE_FILES})
target_link_libraries(${PROJECT_NAME} mdf progress)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
