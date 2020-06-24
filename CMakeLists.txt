cmake_minimum_required(VERSION 3.15)
project(MDFSuper)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
        add_compile_options("/Zc:externConstexpr")
    endif()
    set(Boost_USE_STATIC_RUNTIME OFF)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(Boost_USE_STATIC_RUNTIME ON)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Configure global options.
set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(External)

if (TARGET ExternalDependencies)
    message("Missing external dependencies")
else ()
    add_subdirectory(GenericFiles)
    add_subdirectory(Library)
    add_subdirectory(Tools)
endif ()

