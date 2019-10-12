
# NOTE: On windows with MinGW-64, create a hardlink from the msys install location to the default MinGW location
#       Run in an administrative cmd prompt:
#       mklink /J C:\MinGW "mingwinstalllocation"

# Setup toolchain.
if(MINGW)
    set(BOOST_TOOLCHAIN mingw)
    message("Building boost usings MinGW")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(BOOST_TOOLCHAIN gcc)
    message("Building boost using GCC")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(BOOST_TOOLCHAIN clang)
    message("Building boost using Clang")
endif()

# Setup paths.
set(BOOST_PREFIX_DIR ${EXTERNAL_PROJECTS_DIR}/Boost)
set(BOOST_SOURCE_DIR ${BOOST_PREFIX_DIR}/src/boost)
set(BOOST_BUILD_DIR ${BOOST_PREFIX_DIR}/src/build)
set(BOOST_INSTALL_DIR ${BOOST_PREFIX_DIR}/root)

# Configure which executable to call during configure.
if(WIN32)
    set(BOOST_CONFIGURE_COMMAND bootstrap.bat ${BOOST_TOOLCHAIN})
else()
    set(BOOST_CONFIGURE_COMMAND ./bootstrap.sh ${BOOST_TOOLCHAIN})
endif()

set(BUILD_COMMAND_ARGS
    --with-log
    --with-filesystem
    --with-thread
    --with-program_options
    --with-headers
    --prefix=${BOOST_INSTALL_DIR}
    toolset=${BOOST_TOOLCHAIN}
    threading=multi
    install
    -j2
    )

# Configure arguments to b2 when building.
if(WIN32)
    set(BOOST_BUILD_COMMAND
        b2.exe
        ${BUILD_COMMAND_ARGS}
        variant=debug,release
        address-model=32,64
        runtime-link=static
        )
else()
    set(BOOST_BUILD_COMMAND
        ./b2
        ${BUILD_COMMAND_ARGS}
        variant=release
        address-model=64
        link=shared
        install
        )
endif()

# Finally, gather all the commands
ExternalProject_Add(boost_builder
    PREFIX ${BOOST_PREFIX_DIR}
    URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz
    URL_MD5 "5f521b41b79bf8616582c4a8a2c10177"
    SOURCE_DIR ${BOOST_SOURCE_DIR}
    INSTALL_DIR ${BOOST_INSTALL_DIR}
    CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND}
    BUILD_COMMAND ${BOOST_BUILD_COMMAND}
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
    )

set(Boost_NO_SYSTEM_PATHS TRUE CACHE INTERNAL "Boost use system paths" FORCE)
set(BOOST_ROOT "${BOOST_INSTALL_DIR}" CACHE INTERNAL "" FORCE)
