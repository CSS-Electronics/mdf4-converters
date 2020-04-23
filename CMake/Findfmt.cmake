# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

if (NOT fmt_ROOT)
    set(fmt_ROOT "$ENV{fmt_ROOT}")
endif ()

if (NOT fmt_ROOT)
    find_path(_fmt_ROOT NAMES include/fmt/core.h)
else ()
    set(_fmt_ROOT ${fmt_ROOT})
endif ()

find_path(fmt_INCLUDE_DIR
    NAMES core.h
    HINTS ${_fmt_ROOT}/include/fmt
    )

if (NOT ${fmt_INCLUDE_DIR} STREQUAL "fmt_INCLUDE_DIR-NOTFOUND")
    # Select the parent dir, such that all includes are in the form of "fmt/header"
    get_filename_component(fmt_INCLUDE_DIR ${fmt_INCLUDE_DIR} DIRECTORY)

    # Path to common file with configuration parameters.
    set(_fmt_H ${fmt_INCLUDE_DIR}/fmt/core.h)

    # Function to extract version information from the header.
    function(_fmt_EXTRACT _fmt_VER_MAJOR_OUTPUT _fmt_VER_MINOR_OUTPUT _fmt_VER_PATCH_OUTPUT)
        set(CMAKE_MATCH_1 "0")
        set(CMAKE_MATCH_2 "0")
        set(CMAKE_MATCH_3 "0")
        set(_fmt_expr "^[ \\t]*#define[ \\t]+FMT_VERSION[ \\t]+([0-9])([0-9][0-9])([0-9][0-9])")
        file(STRINGS "${_fmt_H}" _fmt_ver REGEX "${_fmt_expr}")
        string(REGEX MATCH "${_fmt_expr}" fmt_ver "${_fmt_ver}")
        string(REPLACE "0" "" _match_1 ${CMAKE_MATCH_1})
        string(REPLACE "0" "" _match_2 ${CMAKE_MATCH_2})
        string(REPLACE "0" "" _match_3 ${CMAKE_MATCH_3})
        set(${_fmt_VER_MAJOR_OUTPUT} "${_match_1}" PARENT_SCOPE)
        set(${_fmt_VER_MINOR_OUTPUT} "${_match_2}" PARENT_SCOPE)
        set(${_fmt_VER_PATCH_OUTPUT} "${_match_3}" PARENT_SCOPE)
    endfunction()

    # Extract total version information.
    _fmt_EXTRACT(FMT_VERSION_MAJOR FMT_VERSION_MINOR FMT_VERSION_PATCH)

    # Split into major, minor and patch.


    if (fmt_FIND_VERSION_COUNT GREATER 2)
        set(fmt_VERSION "${FMT_VERSION_MAJOR}.${FMT_VERSION_MINOR}.${FMT_VERSION_PATCH}")
    else ()
        set(fmt_VERSION "${FMT_VERSION_MAJOR}.${FMT_VERSION_MINOR}")
    endif ()

    if (NOT ${CMAKE_C_PLATFORM_ID} STREQUAL "Windows")
        find_library(fmt_LIBRARY
            NAMES
            fmt
            fmtd
            HINTS
            ${_fmt_ROOT}/lib
            ${_fmt_ROOT}/lib/x86_64-linux-gnu
            )
    else ()
        find_library(fmt_LIBRARY
            NAMES
            fmt
            fmtd
            HINTS
            ${_fmt_ROOT}/lib
            ${_fmt_ROOT}/lib64
            )
    endif ()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(fmt
        FOUND_VAR
        fmt_FOUND
        REQUIRED_VARS
        fmt_INCLUDE_DIR
        fmt_LIBRARY
        VERSION_VAR
        fmt_VERSION
        )

    if (fmt_FOUND)
        # Export variables.
        set(fmt_LIBRARIES ${fmt_LIBRARY})
        set(fmt_INCLUDE_DIRS ${fmt_INCLUDE_DIR})

        # Create target if not already present.
        if (NOT TARGET fmt::fmt)
            add_library(fmt::fmt UNKNOWN IMPORTED)
            set_target_properties(fmt::fmt PROPERTIES
                IMPORTED_LOCATION "${fmt_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${fmt_INCLUDE_DIR}"
                )
        else ()
            print_target_properties(fmt::fmt)
        endif ()

    endif ()
endif ()
