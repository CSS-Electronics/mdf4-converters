# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

function(print_properties)
    message ("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction(print_properties)

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
        message("There is no target named '${tgt}'")
        return()
    endif()

    foreach (prop ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        # Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
        if(prop STREQUAL "LOCATION" OR prop MATCHES "^LOCATION_" OR prop MATCHES "_LOCATION$")
            continue()
        endif()
        # message ("Checking ${prop}")
        get_property(propval TARGET ${tgt} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${tgt} ${prop})
            message ("${tgt} ${prop} = ${propval}")
        endif()
    endforeach(prop)
endfunction(print_target_properties)

if (NOT python_ROOT)
    set(python_ROOT "$ENV{python_ROOT}")
endif ()

if (NOT python_ROOT)
    find_path(_python_ROOT NAMES include/fmt/core.h)
else ()
    set(_python_ROOT ${python_ROOT})
endif ()

find_path(PythonMulti_INCLUDE_DIR
    NAMES python.h
    HINTS ${_python_ROOT}/include
    )

if (NOT ${PythonMulti_INCLUDE_DIR} STREQUAL "python_INCLUDE_DIR-NOTFOUND")
    # Path to common file with configuration parameters.
    set(_python_H ${PythonMulti_INCLUDE_DIR}/Python.h)
    set(_pathlevel_H ${PythonMulti_INCLUDE_DIR}/patchlevel.h)

    # Function to extract version information from the header.
    function(_python_EXTRACT _python_VER_MAJOR_OUTPUT _python_VER_MINOR_OUTPUT _python_VER_PATCH_OUTPUT)
        set(CMAKE_MATCH_1 "0")
        set(CMAKE_MATCH_2 "0")
        set(CMAKE_MATCH_3 "0")
        set(_python_expr "^[ \\t]*#define[ \\t]+PY_VERSION[ \\t]+\"([0-9]).([0-9]+).([0-9]+)\"")
        file(STRINGS "${_pathlevel_H}" _python_ver REGEX "${_python_expr}")
        string(REGEX MATCH "${_python_expr}" python_ver "${_python_ver}")
        string(REPLACE "0" "" _match_1 ${CMAKE_MATCH_1})
        string(REPLACE "0" "" _match_2 ${CMAKE_MATCH_2})
        string(REPLACE "0" "" _match_3 ${CMAKE_MATCH_3})
        set(${_python_VER_MAJOR_OUTPUT} "${_match_1}" PARENT_SCOPE)
        set(${_python_VER_MINOR_OUTPUT} "${_match_2}" PARENT_SCOPE)
        set(${_python_VER_PATCH_OUTPUT} "${_match_3}" PARENT_SCOPE)
    endfunction()

    # Extract total version information.
    _python_EXTRACT(PYTHON_VERSION_MAJOR PYTHON_VERSION_MINOR PYTHON_VERSION_PATCH)

    # Split into major, minor and patch.


    if (python_FIND_VERSION_COUNT GREATER 2)
        set(PythonMulti_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}.${PYTHON_VERSION_PATCH}")
    else ()
        set(PythonMulti_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
    endif ()

    find_library(PythonMulti_LIBRARY
        NAMES
        "python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}"
        python38
        python37
        python36
        python35
        python34
        HINTS
        ${_python_ROOT}/libs
        )
    find_library(PythonMulti_generic_LIBRARY
        NAMES
        python3
        HINTS
        ${_python_ROOT}/libs
        )
    find_program(PythonMulti_INTERPRETER
        NAMES
        python
        HINTS
        ${_python_ROOT}
        )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(PythonMulti
        FOUND_VAR
        PythonMulti_FOUND
        REQUIRED_VARS
        PythonMulti_INCLUDE_DIR
        PythonMulti_LIBRARY
        PythonMulti_generic_LIBRARY
        PythonMulti_INTERPRETER
        VERSION_VAR
        PythonMulti_VERSION
        )

    if (PythonMulti_FOUND)
        # Export variables.
        set(PythonMulti_LIBRARIES ${PythonMulti_LIBRARY})
        set(PythonMulti_INCLUDE_DIRS ${PythonMulti_INCLUDE_DIR})

        # Create target if not already present.
        if (NOT TARGET PythonMulti::Module)
            add_library(PythonMulti::Module UNKNOWN IMPORTED)
            set_target_properties(PythonMulti::Module PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PythonMulti_INCLUDE_DIR}"
                )
        endif ()
        if (NOT TARGET PythonMulti::Generic)
            add_library(PythonMulti::Generic UNKNOWN IMPORTED)
            set_target_properties(PythonMulti::Generic PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_generic_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PythonMulti_INCLUDE_DIR}"
                )
        endif ()
        if (NOT TARGET PythonMulti::Interpreter)
            add_executable(PythonMulti::Interpreter IMPORTED)
            set_target_properties(PythonMulti::Interpreter PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_INTERPRETER}"
                )
        endif ()
        message("Found generic python ${PythonMulti_VERSION}")
    endif ()
endif ()
