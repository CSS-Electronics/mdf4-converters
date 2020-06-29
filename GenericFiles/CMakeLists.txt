project(GenericFiles)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)

#set(CMAKE_CXX_STANDARD 17)

# Check for python 3 support.
find_package(Python3 COMPONENTS Development)

# Search for Boost with python support.
find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(Threads REQUIRED)
find_package(Heatshrink 0.4.1 REQUIRED)
find_package(botan 2.12.1 REQUIRED)

# Embed information in the source code.
configure_file(
  Source/ProjectInformation.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Source/ProjectInformation.h
  @ONLY)

# List of all public header files.
set(INCLUDE_FILES
  Include/AESGCMFile.h
  Include/HeatshrinkFile.h
  Include/GenericFile.h
  )

include_directories(Include)

# List of internal source files related to python.
set(SOURCE_PYTHON_MODULE

  )

# List of all core headers.
set(SOURCE_HEADERS

  )

# List of all core source files.
set(SOURCE_FILES
  Source/AESGCMFile.cpp
  Source/GenericFile.cpp
  Source/HeatshrinkFile.cpp
  )

# Create an amalgamated list for sources and target libraries depending on the options.
set(SOURCE_FINAL
  ${SOURCE_FILES}
  ${SOURCE_HEADER_FILES}
  ${INCLUDE_FILES}
  )

set(LIBRARIES_FINAL
  Boost::headers
  Threads::Threads
  botan::botan
  Heatshrink::Heatshrink
  )

# Update list depending on located optional components.
if(Boost_PYTHON_FOUND AND Python3_Development_FOUND)
  message("Boost python support located along with python 3 development files, building python module")

  string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")

  list(APPEND SOURCE_FINAL ${SOURCE_PYTHON_MODULE})
  list(APPEND LIBRARIES_FINAL
    Python3::Module
    Python3::Python
    Boost::python
    )
endif()

# Create object library of files.
add_library(${PROJECT_NAME}_objlib OBJECT ${SOURCE_FINAL})
set_property(TARGET ${PROJECT_NAME}_objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_link_libraries(${PROJECT_NAME}_objlib ${LIBRARIES_FINAL})

# Create static library.
add_library(${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
target_link_libraries(${PROJECT_NAME}_static ${LIBRARIES_FINAL})

target_include_directories(${PROJECT_NAME}_static INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
  )

# Create dynamic library.
add_library(${PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
target_link_libraries(${PROJECT_NAME}_shared ${LIBRARIES_FINAL})

target_include_directories(${PROJECT_NAME}_shared INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
  )
