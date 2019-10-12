cmake_minimum_required(VERSION 3.7)
project(MDFSimpleConverters)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

include(CMake/Packaging.cmake)

# External dependencies, can should be built prior to main configuration.
add_subdirectory(External)

include_directories(Extern)

# Setup Python globally.
set(Python_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Interpreter)

# Configure the flags used for Boost.
set(Boost_USE_MULTITHREADED ON)
if(MINGW)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
else()
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(Boost_USE_STATIC_LIBS OFF)
    ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
endif()
#set(Boost_DEBUG ON)

# Help Boost detection for mingw.
if(MINGW)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(Boost_ARCHITECTURE "-x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(Boost_ARCHITECTURE "-x32")
    endif()
endif()

find_package(Boost 1.71.0 COMPONENTS filesystem log program_options)

# For debug libraries of Boost, when placed in the same folder, use the following.
if(CMAKE_BUILD_TYPE MATCHES Debug)
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "^optimized")
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "^debug")
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "-s-")
    message("Using debug libraries")
else()
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "^optimized")
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "^debug")
    list(FILTER Boost_LIBRARIES EXCLUDE REGEX "-sd-")
    message("Using non-debug libraries")
endif()

find_package(tinyxml2
    CONFIG
    HINTS ${tinyxml2_DIR_PATH}
    )

# Set system runtime libraries.
if(MINGW)
    set(RUNTIME_LIBRARIES
        -static-libgcc
        -static-libstdc++
        -static
        winpthread
        -dynamic
        )
else()
    set(RUNTIME_LIBRARIES
        -static-libgcc
        -static-libstdc++
        )
endif()

if(tinyxml2_FOUND AND Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${tinyxml2_INCLUDE_DIRS})

    link_directories(${Boost_LIBRARY_DIR})
    link_directories(${tinyxml2_LIBRARY_DIR})

    # Simple library to handle a progess bar.
    add_subdirectory(Progress)

    add_subdirectory(Interfaces)

    # Main MDF library.
    add_subdirectory(Library)

    # Tools (Including tests).
    add_subdirectory(Tools)
else()
    if(NOT ${tinyxml2_FOUND})
        message("Missing tinyxml2")
    endif()

    if(NOT ${Boost_FOUND})
        message("Missing Boost")
    endif()

    if(NOT ${date_FOUND})
        message("Missing date")
    endif()

    message("Run the targets defined in the CMakeLists.txt in External")
endif()

