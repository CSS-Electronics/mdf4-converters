# Add support for packaging.
include(CMake/Package.cmake)

# Set common packaging options.
set(RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(ICON_PATH ${RESOURCE_PATH}/icon.ico)

if (MSVC_VERSION GREATER_EQUAL "1900")
    set(RUNTIME_LIBRARIES "")
else()
    set(RUNTIME_LIBRARIES
            -static-libstdc++
            -static-libgcc
            )
endif()

if(MINGW)
    list(APPEND RUNTIME_LIBRARIES -static)
endif()

# Shared base for all the tools.
add_subdirectory(Shared)

# Each converter tool.
add_subdirectory(ASC)
add_subdirectory(CLX000)
add_subdirectory(CSV)
add_subdirectory(MDF)
add_subdirectory(PEAK)
add_subdirectory(PCAP)
add_subdirectory(SocketCAN)

# Create a meta-target for building a release.
set(EXTERNAL_TARGETS
    mdf2asc
    mdf2clx000
    mdf2csv
    mdf2finalized
    mdf2pcap
    mdf2peak
    mdf2socketcan
    )

add_custom_target(ToolsRelease
    DEPENDS ${EXTERNAL_TARGETS}
    )
