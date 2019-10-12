set(EXTERNAL_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/External)
include(ExternalProject)

include(Boost)
include(tinyxml2)

if (UNIX)
    include(linuxdeploy)
endif (UNIX)
