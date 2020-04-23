include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "botan")

set(botan_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "botan root path")
set(ENV{botan_ROOT} ${botan_ROOT})
find_package(botan 2.12.1)

if(NOT botan_FOUND)
    message("External dependencies: botan not found, creating external target.")

    # Configure helper.
    set(BOTAN_CONFIGURE_COMMAND_OS "")
    if(MINGW)
        set(BOTAN_CONFIGURE_COMMAND_OS "--os=mingw")
    endif()

    set(BOTAN_CONFIGURE_COMMAND_CC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(BOTAN_CONFIGURE_COMMAND_CC "gcc")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(BOTAN_CONFIGURE_COMMAND_CC "clang")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(BOTAN_CONFIGURE_COMMAND_CC "msvc")
    endif()

    find_package(Python COMPONENTS Interpreter REQUIRED)

    set(EXTERNAL_PROJECT_CONFIGURE_COMMAND
      COMMAND ${Python_EXECUTABLE} <SOURCE_DIR>/configure.py --prefix=<INSTALL_DIR> ${BOTAN_CONFIGURE_COMMAND_OS} --cc=${BOTAN_CONFIGURE_COMMAND_CC} --minimized-build --without-stack-protector --enable-modules=system_rng,aead,aes,aes_ni,aes_vperm,gcm,hmac,sha2_32,sha2_32_bmi2,sha2_32_x86,sha2_64,sha2_64_bmi2,sha1,sha3,shacal2,shacal2_simd,shacal2_x86,simd,simd_avx2,pbkdf,pbkdf1,pbkdf2
      )

    # Command used for building.
    set(EXTERNAL_PROJECT_BUILD_COMMAND
      ${CMAKE_BUILD_TOOL}
      )

    # Command used for installation.
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
      ${CMAKE_BUILD_TOOL} install
      )

    ExternalProject_Add (
      ${EXTERNAL_PROJECT_NAME}_builder

      PREFIX            ${EXTERNAL_PROJECT_NAME}
      URL               https://github.com/randombit/botan/archive/2.12.1.tar.gz
      URL_HASH          MD5=6ae722e00cb2d3df5562b0314d2aea90

      CONFIGURE_COMMAND ${EXTERNAL_PROJECT_CONFIGURE_COMMAND}
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
    message("External dependencies: botan found at ${botan_INCLUDE_DIRS}")
endif()
