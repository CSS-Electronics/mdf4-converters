project(mdf2asc)

#set(CMAKE_CXX_STANDARD 17)

# Set project information.
set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to PEAK ASCII format")

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
  Source/ASC_CAN_Exporter.cpp
  Source/ASC_CAN_Exporter.h
  Source/ASC_Exporter.cpp
  Source/ASC_Exporter.h
  Source/ASC_LIN_Exporter.cpp
  Source/ASC_LIN_Exporter.h
  Source/main.cpp
  )

include_directories(Include)
add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME}
    ${RUNTIME_LIBRARIES}
    MDFSimpleConverters_Shared
    fmt::fmt
    )

pack_executable(${PROGRAM_NAME})

