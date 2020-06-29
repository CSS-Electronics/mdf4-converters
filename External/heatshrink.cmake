include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "heatshrink")

set(Heatshrink_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "Heatshrink root path")
set(ENV{Heatshrink_ROOT} ${Heatshrink_ROOT})
find_package(Heatshrink 0.4.1)

if(NOT Heatshrink_FOUND)
  message("Heatshrink not found, creating external target.")

  # Command used for building.
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(EXTERNAL_PROJECT_BUILD_COMMAND
        COMMAND ${CMAKE_C_COMPILER} /c /DHEATSHRINK_DYNAMIC_ALLOC=0 <SOURCE_DIR>/heatshrink_decoder.c
        COMMAND ${CMAKE_C_COMPILER} /c /DHEATSHRINK_DYNAMIC_ALLOC=0 <SOURCE_DIR>/heatshrink_encoder.c
        COMMAND ${CMAKE_C_COMPILER} /LD <SOURCE_DIR>/heatshrink_decoder.obj <SOURCE_DIR>/heatshrink_encoder.obj /Fe<SOURCE_DIR>/heatshrink.lib
        )
  else ()
    set(EXTERNAL_PROJECT_BUILD_COMMAND
        ${CMAKE_BUILD_TOOL} libraries
        )
  endif ()

  # Command used for installation.
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink.lib <INSTALL_DIR>/lib/heatshrink.lib
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_common.h <INSTALL_DIR>/include/heatshrink_common.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_config.h <INSTALL_DIR>/include/heatshrink_config.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_decoder.h <INSTALL_DIR>/include/heatshrink_decoder.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_encoder.h <INSTALL_DIR>/include/heatshrink_encoder.h
        )
  else ()
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/libheatshrink_dynamic.a <INSTALL_DIR>/lib/libheatshrink.a
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_common.h <INSTALL_DIR>/include/heatshrink_common.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_config.h <INSTALL_DIR>/include/heatshrink_config.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_decoder.h <INSTALL_DIR>/include/heatshrink_decoder.h
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/heatshrink_encoder.h <INSTALL_DIR>/include/heatshrink_encoder.h
        )
  endif ()

  ExternalProject_Add (
    ${EXTERNAL_PROJECT_NAME}_builder

    PREFIX            ${EXTERNAL_PROJECT_NAME}
    URL               https://github.com/atomicobject/heatshrink/archive/v0.4.1.tar.gz
    URL_HASH          MD5=54ebdde46b4559b07ec19d95dde5ae9d

    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ${EXTERNAL_PROJECT_BUILD_COMMAND}
    INSTALL_COMMAND   ${EXTERNAL_PROJECT_INSTALL_COMMAND}
    UPDATE_COMMAND    ""

    BUILD_IN_SOURCE   1

    BUILD_ALWAYS      OFF
    INSTALL_DIR       ${EXTERNAL_PROJECT_INSTALL_DIR}
  )

  # Add to targets list.
  list(APPEND EXTERNAL_TARGETS ${EXTERNAL_PROJECT_NAME}_builder)
  set(EXTERNAL_TARGETS ${EXTERNAL_TARGETS} PARENT_SCOPE)
else()
  message("Heatshrink found at ${Heatshrink_INCLUDE_DIRS}")
endif()