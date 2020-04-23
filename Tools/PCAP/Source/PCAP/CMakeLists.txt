project(PCAP CXX)

set(SOURCE_FILES
  PCAP_Block.cpp
  PCAP_Block.h
  PCAP_EnhancedPacket.cpp
  PCAP_EnhancedPacket.h
  PCAP_EnhancedPacket_CAN.cpp
  PCAP_EnhancedPacket_CAN.h
  PCAP_InterfaceDescription.cpp
  PCAP_InterfaceDescription.h
  PCAP_Option.cpp
  PCAP_Option.h
  PCAP_OptionEnd.cpp
  PCAP_OptionEnd.h
  PCAP_OptionList.cpp
  PCAP_OptionList.h
  PCAP_OptionString.cpp
  PCAP_OptionString.h
  PCAP_OptionU32.cpp
  PCAP_OptionU32.h
  PCAP_OptionU64.cpp
  PCAP_OptionU64.h
  PCAP_SectionHeader.cpp
  PCAP_SectionHeader.h
  )

add_library(${PROJECT_NAME} OBJECT ${SOURCE_FILES})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
