project(mdf2clx000)

#set(CMAKE_CXX_STANDARD 17)

# Set project information.
set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to CLX000 format")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 6)

# Set required additional packages.
find_package(fmt REQUIRED)

# Embed information in the source code.
configure_file(
  Source/ProjectInformation.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Source/ProjectInformation.h
  @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

set(SOURCE_FILES
    Source/CLX000_CAN_Exporter.cpp
    Source/CLX000_CAN_Exporter.h
    Source/CLX000_Configuration.h
    Source/CLX000_Exporter.cpp
    Source/CLX000_Exporter.h
    Source/CLX000_LIN_Exporter.cpp
    Source/CLX000_LIN_Exporter.h
    Source/main.cpp
    )

include_directories(Include)
add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME}
    ${RUNTIME_LIBRARIES}
    MDFSimpleConverters_Shared
    fmt::fmt
    )

# Setup additional files.
set(ADDITIONAL_RELEASE_FILES
    config.ini
    README.md
    )

set_property(TARGET ${PROGRAM_NAME}
    PROPERTY ADDITIONAL_FILES ${ADDITIONAL_RELEASE_FILES}
    )

pack_executable(${PROGRAM_NAME})
