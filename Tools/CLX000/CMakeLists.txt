project(mdf2clx000 CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to CLX000 format")

set(SOURCE_FILES
    CLX000_CAN_Exporter.cpp
    CLX000_CAN_Exporter.h
    CLX000_Configuration.h
    CLX000_Exporter.cpp
    CLX000_Exporter.h
    CLX000_LIN_Exporter.cpp
    CLX000_LIN_Exporter.h
    main.cpp
    )

set(ADDITIONAL_RELEASE_FILES
    config.ini
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

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROGRAM_NAME}.map")
