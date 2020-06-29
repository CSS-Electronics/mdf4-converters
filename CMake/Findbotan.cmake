# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if (NOT botan_ROOT)
    set(botan_ROOT "$ENV{botan_ROOT}")
endif ()

if (NOT botan_ROOT)
    find_path(_botan_ROOT NAMES include/botan-2/botan/build.h)
else ()
    set(_botan_ROOT ${botan_ROOT})
endif ()

find_path(botan_INCLUDE_DIR
    NAMES botan/build.h
    HINTS ${_botan_ROOT}/include/botan-2
    )

if (NOT ${botan_INCLUDE_DIR} STREQUAL "botan_INCLUDE_DIR-NOTFOUND")
    # Path to common file with configuration parameters.
    set(_botan_H ${botan_INCLUDE_DIR}/botan/build.h)

    # Function to extract version information from the header.
    function(_botan_EXTRACT _botan_VER_COMPONENT _botan_VER_OUTPUT)
        set(CMAKE_MATCH_1 "0")
        set(_botan_expr "^[ \\t]*#define[ \\t]+${_botan_VER_COMPONENT}[ \\t]+([0-9]+)$")

        if (EXISTS "${_botan_H}")
            file(STRINGS "${_botan_H}" _botan_ver REGEX "${_botan_expr}")
            string(REGEX MATCH "${_botan_expr}" botan_ver "${_botan_ver}")
        endif ()

        set(${_botan_VER_OUTPUT} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    endfunction()

    # Extract version information.
    _botan_EXTRACT("BOTAN_VERSION_MAJOR" BOTAN_VERSION_MAJOR)
    _botan_EXTRACT("BOTAN_VERSION_MINOR" BOTAN_VERSION_MINOR)
    _botan_EXTRACT("BOTAN_VERSION_PATCH" BOTAN_VERSION_PATCH)

    if (botan_FIND_VERSION_COUNT GREATER 2)
        set(botan_VERSION "${BOTAN_VERSION_MAJOR}.${BOTAN_VERSION_MINOR}.${BOTAN_VERSION_PATCH}")
    else ()
        set(botan_VERSION "${BOTAN_VERSION_MAJOR}.${BOTAN_VERSION_MINOR}")
    endif ()

    set(BOTAN_LIBRARY_HINTS ${_botan_ROOT}/lib)
    set(BOTAN_LIBRARY_NAMES)

    if (NOT ${CMAKE_C_PLATFORM_ID} STREQUAL "Windows")
        list(APPEND BOTAN_LIBRARY_NAMES botan-2)
        list(APPEND BOTAN_LIBRARY_HINTS ${_botan_ROOT}/lib/x86_64-linux-gnu)
    else ()
        list(APPEND BOTAN_LIBRARY_NAMES botan)
    endif ()

    find_library(botan_LIBRARY NAMES ${BOTAN_LIBRARY_NAMES} HINTS ${BOTAN_LIBRARY_HINTS})
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(botan
    FOUND_VAR
    botan_FOUND
    REQUIRED_VARS
    botan_INCLUDE_DIR
    botan_LIBRARY
    VERSION_VAR
    botan_VERSION
    )

if (botan_FOUND)
    # Export variables.
    set(botan_LIBRARIES ${botan_LIBRARY})
    set(botan_INCLUDE_DIRS ${botan_INCLUDE_DIR})

    # Create target if not already present.
    if (NOT TARGET botan::botan)
        add_library(botan::botan UNKNOWN IMPORTED)
        set_target_properties(botan::botan PROPERTIES
            IMPORTED_LOCATION "${botan_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${botan_INCLUDE_DIR}"
            )
    endif ()

endif ()