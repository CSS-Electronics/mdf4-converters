project(GenericFiles)

set(GenericFiles_VERSION_MAJOR 1)
set(GenericFiles_VERSION_MINOR 0)
set(GenericFiles_VERSION_PATCH 3)

find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(Threads REQUIRED)
find_package(Heatshrink 0.4.1 REQUIRED)
find_package(botan 2.12.1 REQUIRED)

find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp")

# Embed information in the source code.
configure_file(
    Source/ProjectInformation.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProjectInformation.h
    @ONLY)

# List of all public header files.
set(INCLUDE_FILES
    Include/AESGCMFile.h
    Include/HeatshrinkFile.h
    Include/GenericFile.h
    )

include_directories(Include)

# List of internal source files related to python.
set(SOURCE_PYTHON_MODULE

    )

# List of all core headers.
set(SOURCE_HEADERS

    )

# List of all core source files.
set(SOURCE_FILES
    Source/AESGCMFile.cpp
    Source/GenericFile.cpp
    Source/HeatshrinkFile.cpp
    )

# Create an amalgamated list for sources and target libraries depending on the options.
set(SOURCE_FINAL
    ${SOURCE_FILES}
    ${SOURCE_HEADER_FILES}
    ${INCLUDE_FILES}
    )

set(LIBRARIES_FINAL
    Boost::headers
    Threads::Threads
    botan::botan
    Heatshrink::Heatshrink
    )

# Create static library.
add_library(${PROJECT_NAME} STATIC ${SOURCE_FINAL})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_FINAL})

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
target_include_directories(${PROJECT_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
