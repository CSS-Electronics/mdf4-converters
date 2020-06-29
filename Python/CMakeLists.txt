cmake_minimum_required(VERSION 3.15)
project(MDF_BusLogging_PythonModule)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Search for Python.
if($ENV{CI})
    set(python_ROOT $ENV{pythonLocation} CACHE INTERNAL "Python path")
    message("Detected Github Actions CI environment, using ${Python_ROOT_DIR} for python location")
elseif($ENV{python_ROOT})
    set(python_ROOT $ENV{pythonLocation} CACHE INTERNAL "Python path")
endif()

find_package(PythonMulti COMPONENTS Interpreter Development REQUIRED)

find_package(Threads)
find_package(PyCXX REQUIRED)

# List of all source files.
set(SOURCE_FILES
    Source/CallbackBuffer.cpp
    Source/CallbackBuffer.h
    Source/Export.cpp
    Source/MdfFileWrapper.cpp
    Source/MdfFileWrapper.h
    Source/MdfCANRecordWrapper.cpp
    Source/MdfCANRecordWrapper.h
    Source/MdfCANIteratorWrapper.cpp
    Source/MdfCANIteratorWrapper.h
    Source/PythonLogger.cpp
    Source/PythonLogger.h
    Source/TypeSupport.cpp
    Source/TypeSupport.h

    Source/Module.cpp
    Source/Module.h
    )

if (MSVC_VERSION GREATER_EQUAL "1900")
    add_compile_definitions(HAVE_SNPRINTF=1)
endif()

# Create python extension.
set(LIBRARY_NAME mdf_iter)
add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIBRARY_NAME}
    Threads::Threads
    PyCXX::PyCXX
    MDF_BusLogging_Library_static
    PythonMulti::Module
    PythonMulti::Generic
    )

if (MSVC_VERSION GREATER_EQUAL "1900")
    set_target_properties(${LIBRARY_NAME} PROPERTIES SUFFIX ".pyd")
endif()
set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")
add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIBRARY_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator/mdf_iter
    )

# Build wheel after module.
add_custom_target(${LIBRARY_NAME}_wheel
        COMMAND PythonMulti::Interpreter setup.py bdist_wheel
        DEPENDS ${LIBRARY_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Distribution_Iterator
        )
