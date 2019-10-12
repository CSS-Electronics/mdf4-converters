project(mdf2pcap CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to PCAP-NG format")

set(SOURCE_FILES
    PCAP_CAN_Exporter.cpp
    PCAP_CAN_Exporter.h
    PCAP_Exporter.cpp
    PCAP_Exporter.h
    main.cpp

    PCAP/PCAP_Block.cpp
    PCAP/PCAP_Block.h
    PCAP/PCAP_EnhancedPacket.cpp
    PCAP/PCAP_EnhancedPacket.h
    PCAP/PCAP_EnhancedPacket_CAN.cpp
    PCAP/PCAP_EnhancedPacket_CAN.h
    PCAP/PCAP_InterfaceDescription.cpp
    PCAP/PCAP_InterfaceDescription.h
    PCAP/PCAP_Option.cpp
    PCAP/PCAP_Option.h
    PCAP/PCAP_OptionEnd.cpp
    PCAP/PCAP_OptionEnd.h
    PCAP/PCAP_OptionList.cpp
    PCAP/PCAP_OptionList.h
    PCAP/PCAP_OptionString.cpp
    PCAP/PCAP_OptionString.h
    PCAP/PCAP_OptionU32.cpp
    PCAP/PCAP_OptionU32.h
    PCAP/PCAP_OptionU64.cpp
    PCAP/PCAP_OptionU64.h
    PCAP/PCAP_SectionHeader.cpp
    PCAP/PCAP_SectionHeader.h
    )

set(ADDITIONAL_RELEASE_FILES
    README.md
    )

# Embed information in the source code.
configure_file(ProjectInformation.h.in ProjectInformation.h @ONLY)
set(LINK_TARGETS
        ToolsBase
        mdf
        ${RUNTIME_LIBRARIES}
        ${Boost_LIBRARIES}
        )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROGRAM_NAME} ${SOURCE_FILES})
target_link_libraries(${PROGRAM_NAME} ${LINK_TARGETS})

set_property(TARGET ${PROGRAM_NAME}
    PROPERTY ADDITIONAL_FILES ${ADDITIONAL_RELEASE_FILES}
    )

pack_executable(${PROGRAM_NAME})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")
