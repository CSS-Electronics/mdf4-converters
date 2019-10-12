project(interfaces CXX)

include_directories(Include)

set(SOURCE_FILES
    Version.cpp

    Include/Version.h
    )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
