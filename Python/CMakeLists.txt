cmake_minimum_required(VERSION 3.15)
project(MDF_BusLogging_PythonModule)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)

cmake_policy(SET CMP0094 NEW)
set(Python3_FIND_STRATEGY "LOCATION")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Search for Python.
find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)
find_package(Threads)

if(WIN32)
    # The target output is an ABI3 compatible library, targetting the limited Python ABI. The default seems to be to
    # link against the generic libraries, so this block replaces the full version name with the generic version names.

    # Alter the Python targets to go after the generic and not the specific edition.
    get_property(PYTHON_LOCATION_DLL TARGET Python3::Module PROPERTY IMPORTED_LOCATION)
    get_property(PYTHON_LOCATION_LIB TARGET Python3::Module PROPERTY IMPORTED_IMPLIB)

    # Get full component.
    get_filename_component(PYTHON_FILENAME_DLL ${PYTHON_LOCATION_DLL} NAME_WE)
    get_filename_component(PYTHON_FILENAME_LIB ${PYTHON_LOCATION_DLL} NAME_WE)

    string(REGEX REPLACE "python3[0-9]." "python3." PYTHON_LOCATION_DLL ${PYTHON_LOCATION_DLL})
    string(REGEX REPLACE "python3[0-9]." "python3." PYTHON_LOCATION_LIB ${PYTHON_LOCATION_LIB})

    set_property(TARGET Python3::Module PROPERTY IMPORTED_LOCATION ${PYTHON_LOCATION_DLL})
    set_property(TARGET Python3::Module PROPERTY IMPORTED_IMPLIB ${PYTHON_LOCATION_LIB})
endif()


# Setup PyCXX target.
set(PyCXX_ROOT ${CMAKE_CURRENT_BINARY_DIR}/External/PyCXX CACHE PATH "PyCXX root path")
set(ENV{PyCXX_ROOT} ${PyCXX_ROOT})
include(PyCXX.cmake)

# Find external dependencies.
find_package(PyCXX 7.1.5)

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
    Python3::Module
    MDF_BusLogging_Library
)

set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")

# Remove unneeded symbols in release builds (Not necessary on Windows).
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE STREQUAL Release)
    add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${LIBRARY_NAME}>)
endif()

# Copy artifact into source tree for building the wheel later.
add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIBRARY_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator/mdf_iter
    )

# NOTE: PyCXX is not ready for C++20 yet.
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${LIBRARY_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${LIBRARY_NAME} PROPERTY SUFFIX ".pyd")
    set_property(TARGET ${LIBRARY_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Build wheel after module.
add_custom_target(${LIBRARY_NAME}_wheel
        COMMAND Python3::Interpreter setup.py bdist_wheel --py-limited-api=cp35
        DEPENDS ${LIBRARY_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator
        )
endif()
