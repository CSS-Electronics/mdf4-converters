include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "Boost")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(BOOST_TOOLCHAIN gcc)
    set(Boost_COMPILER ${BOOST_TOOLCHAIN} CACHE INTERNAL "Boost compiler")
    message("Building boost using GCC")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(BOOST_TOOLCHAIN clang)
    set(Boost_COMPILER ${BOOST_TOOLCHAIN} CACHE INTERNAL "Boost compiler")
    message("Building boost using Clang")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(BOOST_TOOLCHAIN msvc)
    set(Boost_COMPILER "-vc142" CACHE INTERNAL "Boost compiler")
    message("Building boost using MSVC")
else()
    message("ERROR")
endif()

if(NOT DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "Boost root path")
    set(ENV{BOOST_ROOT} ${BOOST_ROOT})
else()
    message("Getting Boost root from environment variable")
    set(BOOST_ROOT $ENV{BOOST_ROOT})
    set(Boost_NO_SYSTEM_PATHS ON)
endif()

message("Using ${BOOST_ROOT} as search path")

set(ENV{Boost_COMPILER} ${Boost_COMPILER})

set(Boost_COMPONENTS_TO_FIND
        date_time
        filesystem
        iostreams
        log
        program_options
        thread
        )

find_package(Boost 1.73.0 COMPONENTS ${Boost_COMPONENTS_TO_FIND})

if(NOT Boost_FOUND)
    message("External dependencies: Boost not found, creating external target.")

    # Setup toolchain.
    if(MINGW)
        set(BOOST_BOOTSTRAP_TOOLCHAIN mingw)
        message("Bootstraping boost using MinGW")
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(BOOST_TOOLCHAIN gcc)
        message("Building boost using GCC")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(BOOST_TOOLCHAIN clang)
        message("Building boost using Clang")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(BOOST_TOOLCHAIN msvc)
        message("Building boost using MSVC")
    else()
        message("ERROR")
    endif()

    if(NOT DEFINED BOOST_BOOTSTRAP_TOOLCHAIN)
        set(BOOST_BOOTSTRAP_TOOLCHAIN ${BOOST_TOOLCHAIN})
    endif()

    set(BUILD_COMMAND_ARGS
        --with-date_time
        --with-filesystem
        --with-headers
        --with-iostreams
        --with-log
        --with-program_options
        --with-thread
        --prefix=<INSTALL_DIR>
        toolset=${BOOST_TOOLCHAIN}
        variant=release
        address-model=64
        threading=multi
        link=static
        runtime-link=static
        install
        -j1
        )

    # Configure arguments to b2 when building.
    if(WIN32)
        set(BOOST_BUILD_COMMAND
            b2.exe
            ${BUILD_COMMAND_ARGS}
            )
    else()
        set(BOOST_BUILD_COMMAND
            ./b2
            ${BUILD_COMMAND_ARGS}
            )
    endif()

    # Configure which executable to call during configure.
    if(WIN32)
        set(BOOST_CONFIGURE_COMMAND bootstrap.bat ${BOOST_BOOTSTRAP_TOOLCHAIN})
    else()
        set(BOOST_CONFIGURE_COMMAND ./bootstrap.sh ${BOOST_BOOTSTRAP_TOOLCHAIN})
    endif()

    ExternalProject_Add (
        ${EXTERNAL_PROJECT_NAME}_builder

        PREFIX            ${EXTERNAL_PROJECT_NAME}
        URL               https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.gz
        URL_HASH          MD5=4036cd27ef7548b8d29c30ea10956196

        CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND}
        BUILD_COMMAND     ${BOOST_BUILD_COMMAND}
        INSTALL_COMMAND   ""

        BUILD_IN_SOURCE 1

        BUILD_ALWAYS      OFF
        INSTALL_DIR       ${EXTERNAL_PROJECT_INSTALL_DIR}
    )

    # Add to targets list.
    list(APPEND EXTERNAL_TARGETS ${EXTERNAL_PROJECT_NAME}_builder)
    set(EXTERNAL_TARGETS ${EXTERNAL_TARGETS} PARENT_SCOPE)
else()
    message("External dependencies: Boost found at ${Boost_INCLUDE_DIR}")
endif()
