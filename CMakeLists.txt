cmake_minimum_required(VERSION 3.15)
project(MDFSuper)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Configure global options.
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

set(Python3_FIND_STRATEGY "LOCATION")

add_subdirectory(GenericFiles)
add_subdirectory(Library)
add_subdirectory(Tools)
add_subdirectory(Python)
