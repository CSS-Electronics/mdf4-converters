project(progress CXX)

set(SOURCE_FILES
    ProgressInterface.cpp
    ProgressInterface.h
    )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    )
