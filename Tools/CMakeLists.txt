# Shared components among all exporters.
add_subdirectory(Shared)

# Common properties for resources related to packing.
set(COMPANY_NAME "CSS Electronics")
set(ICON_PATH ${CMAKE_SOURCE_DIR}/Resources/icon.ico)

define_property(TARGET
    PROPERTY ADDITIONAL_FILES
    BRIEF_DOCS "Additional files to copy over into a release folder."
    FULL_DOCS "Additional files to copy over into a release folder."
    )

# List of all the custom exporters.
add_subdirectory(ASC)
add_subdirectory(CLX000)
add_subdirectory(CSV)
add_subdirectory(PCAP)
add_subdirectory(SocketCAN)
add_subdirectory(Tool)

set(TOOL_TARGETS
    mdf2asc
    mdf2clx000
    mdf2csv
    mdf2finalized
    mdf2pcap
    mdf2socketcan
    )

# Create a target building all the tools.
add_custom_target(Tools)
add_custom_target(Tools_Pack)

set(ADDITIONAL_RELEASE_FILES
    README.md
    LICENSE.md
    )

set_property(TARGET Tools_Pack PROPERTY ADDITIONAL_FILES ${ADDITIONAL_RELEASE_FILES})
copy_additional_files(Tools_Pack)

foreach(TARGET ${TOOL_TARGETS})
    add_dependencies(Tools ${TARGET})
    add_dependencies(Tools_Pack ${TARGET}_install)
endforeach()
