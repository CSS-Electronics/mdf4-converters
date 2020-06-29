project(mdf2pcap CXX)

# Set project information.
set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to PCAP-NG format")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 7)

# Set required additional packages.
find_package(fmt REQUIRED)
find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp" REQUIRED)

# Embed information in the source code.
configure_file(
    Source/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Source/Version.h
    @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

# List all source files.
set(SOURCE_FILES_TOOL
    Source/PCAP_CAN_Exporter.cpp
    Source/PCAP_CAN_Exporter.h
    Source/PCAP_Exporter.cpp
    Source/PCAP_Exporter.h
    Source/main.cpp
    )

set(SOURCE_FILES_PCAP
    Source/PCAP/PCAP_Block.cpp
    Source/PCAP/PCAP_Block.h
    Source/PCAP/PCAP_EnhancedPacket.cpp
    Source/PCAP/PCAP_EnhancedPacket.h
    Source/PCAP/PCAP_EnhancedPacket_CAN.cpp
    Source/PCAP/PCAP_EnhancedPacket_CAN.h
    Source/PCAP/PCAP_InterfaceDescription.cpp
    Source/PCAP/PCAP_InterfaceDescription.h
    Source/PCAP/PCAP_Option.cpp
    Source/PCAP/PCAP_Option.h
    Source/PCAP/PCAP_OptionEnd.cpp
    Source/PCAP/PCAP_OptionEnd.h
    Source/PCAP/PCAP_OptionList.cpp
    Source/PCAP/PCAP_OptionList.h
    Source/PCAP/PCAP_OptionString.cpp
    Source/PCAP/PCAP_OptionString.h
    Source/PCAP/PCAP_OptionU32.cpp
    Source/PCAP/PCAP_OptionU32.h
    Source/PCAP/PCAP_OptionU64.cpp
    Source/PCAP/PCAP_OptionU64.h
    Source/PCAP/PCAP_SectionHeader.cpp
    Source/PCAP/PCAP_SectionHeader.h
    )

set(SOURCE_FILES
    ${SOURCE_FILES_TOOL}
    ${SOURCE_FILES_PCAP}
    )

add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME}
    ${RUNTIME_LIBRARIES}
    MDFSimpleConverters_Shared
    fmt::fmt
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )

pack_executable(${PROGRAM_NAME})
