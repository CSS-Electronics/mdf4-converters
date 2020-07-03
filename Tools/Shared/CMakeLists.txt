project(MDFSimpleConverters_Shared)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS filesystem program_options log REQUIRED)
find_package(Heatshrink 0.4.1 REQUIRED)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 6)

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
    Include/ParseOptionStatus.h
    Include/PasswordStorage.h
    Include/ProgressIndicator.h
    Include/TimeConverter.h
    )

set(SOURCE_FILES
    Source/ConverterInterface.cpp
    Source/ExecutableEntryPoint.cpp
    Source/ExecutableInterface.cpp
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
    MDF_BusLogging_Library::static
    GenericFiles_static
    Boost::log
    Boost::program_options
    ${CMAKE_DL_LIBS}
    )

# Setup custom include paths.
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
