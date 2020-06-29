cmake_minimum_required(VERSION 3.15)
project(MDF_BusLogging_PythonModule)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Search for Python.
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Threads)

# Setup PyCXX target.
set(PyCXX_ROOT ${CMAKE_CURRENT_BINARY_DIR}/External/PyCXX CACHE PATH "PyCXX root path")
set(ENV{PyCXX_ROOT} ${PyCXX_ROOT})
include(PyCXX.cmake)

# Find external dependencies.
find_package(PyCXX)

if(PyCXX_FOUND)
# List of all source files.
set(SOURCE_FILES
    Source/CallbackBuffer.cpp
    Source/CallbackBuffer.h
    Source/Export.cpp
    Source/MdfFileWrapper.cpp
    Source/MdfFileWrapper.h
    Source/PythonLogger.cpp
    Source/PythonLogger.h
    Source/TypeSupport.cpp
    Source/TypeSupport.h

    Source/IteratorWrappers/MDF_CAN_DataFrameIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_CAN_DataFrameIteratorWrapper.h
    Source/IteratorWrappers/MDF_CAN_ErrorFrameIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_CAN_ErrorFrameIteratorWrapper.h
    Source/IteratorWrappers/MDF_CAN_RemoteFrameIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_CAN_RemoteFrameIteratorWrapper.h
    Source/IteratorWrappers/MDF_LIN_ChecksumErrorIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_LIN_ChecksumErrorIteratorWrapper.h
    Source/IteratorWrappers/MDF_LIN_FrameIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_LIN_FrameIteratorWrapper.h
    Source/IteratorWrappers/MDF_LIN_ReceiveErrorIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_LIN_ReceiveErrorIteratorWrapper.h
    Source/IteratorWrappers/MDF_LIN_SyncErrorIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_LIN_SyncErrorIteratorWrapper.h
    Source/IteratorWrappers/MDF_LIN_TransmissionErrorIteratorWrapper.cpp
    Source/IteratorWrappers/MDF_LIN_TransmissionErrorIteratorWrapper.h

    Source/RecordWrappers/MDF_CAN_DataFrameWrapper.cpp
    Source/RecordWrappers/MDF_CAN_DataFrameWrapper.h
    Source/RecordWrappers/MDF_CAN_ErrorFrameWrapper.cpp
    Source/RecordWrappers/MDF_CAN_ErrorFrameWrapper.h
    Source/RecordWrappers/MDF_CAN_RemoteFrameWrapper.cpp
    Source/RecordWrappers/MDF_CAN_RemoteFrameWrapper.h
    Source/RecordWrappers/MDF_LIN_ChecksumErrorWrapper.h
    Source/RecordWrappers/MDF_LIN_ChecksumErrorWrapper.cpp
    Source/RecordWrappers/MDF_LIN_FrameWrapper.cpp
    Source/RecordWrappers/MDF_LIN_FrameWrapper.h
    Source/RecordWrappers/MDF_LIN_ReceiveErrorWrapper.h
    Source/RecordWrappers/MDF_LIN_ReceiveErrorWrapper.cpp
    Source/RecordWrappers/MDF_LIN_SyncErrorWrapper.h
    Source/RecordWrappers/MDF_LIN_SyncErrorWrapper.cpp
    Source/RecordWrappers/MDF_LIN_TransmissionErrorWrapper.cpp
    Source/RecordWrappers/MDF_LIN_TransmissionErrorWrapper.h

    Source/Module.cpp
    Source/Module.h
    )

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_definitions(HAVE_SNPRINTF=1)
endif()

# Create python extension.
link_directories(${Python_LIBRARY_DIRS})
set(LIBRARY_NAME mdf_iter)
add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIBRARY_NAME}
    Threads::Threads
    PyCXX::PyCXX
    MDF_BusLogging_Library
    Python3::Module
)

set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")

# Remove unneeded symbols in release builds (Not on Windows).
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE STREQUAL Release)
    add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${LIBRARY_NAME}>)
endif()

add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIBRARY_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator/mdf_iter
    )

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${LIBRARY_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${LIBRARY_NAME} PROPERTY SUFFIX ".pyd")
    set_property(TARGET ${LIBRARY_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Build wheel after module.
set(WHEEL_ADDITIONAL_ARGUMENTS "")
if (UNIX)
    list(APPEND WHEEL_ADDITIONAL_ARGUMENTS "--py-limited-api cp35")
endif (UNIX)

add_custom_target(${LIBRARY_NAME}_wheel
        COMMAND Python3::Interpreter setup.py bdist_wheel ${WHEEL_ADDITIONAL_ARGUMENTS}
        DEPENDS ${LIBRARY_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator
        )
endif()
