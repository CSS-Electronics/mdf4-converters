project(mdf2asc CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(PROGRAM_NAME ${PROJECT_NAME})
set(DESCRIPTION "MDF4 bus logging to PEAK ASCII format")

set(SOURCE_FILES
    ASC_CAN_Exporter.cpp
    ASC_CAN_Exporter.h
    ASC_Exporter.cpp
    ASC_Exporter.h
    ASC_LIN_Exporter.cpp
    ASC_LIN_Exporter.h
    main.cpp
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

pack_executable(${PROGRAM_NAME})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")