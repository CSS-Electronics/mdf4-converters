cmake_minimum_required(VERSION 3.15)
project(MDFSuper)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Configure global options.

# When using MinGW, use static linkage.
if (MINGW)
    SET(Boost_USE_STATIC_LIBS ON)
    SET(Boost_USE_STATIC_RUNTIME ON)
endif ()

add_subdirectory(External)

if (TARGET ExternalDependencies)
    message("Missing external dependencies")
else ()
    add_subdirectory(GenericFiles)
    add_subdirectory(Library)
    add_subdirectory(Tools)
endif ()

