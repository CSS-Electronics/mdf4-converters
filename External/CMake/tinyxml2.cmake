# Setup paths.
set(TINYXML2_PREFIX_DIR ${EXTERNAL_PROJECTS_DIR}/tinyxml2)
set(TINYXML2_SOURCE_DIR ${TINYXML2_PREFIX_DIR}/src/tinyxml2)
set(TINYXML2_INSTALL_DIR ${TINYXML2_PREFIX_DIR}/root)

# Finally, gather all the commands
ExternalProject_Add(tinyxml2_builder
    PREFIX ${TINYXML2_PREFIX_DIR}
    URL https://github.com/leethomason/tinyxml2/archive/7.1.0.tar.gz
    URL_MD5 "8a02c592702dcc7a061932425d8f3087"
    SOURCE_DIR ${TINYXML2_SOURCE_DIR}
    INSTALL_DIR ${TINYXML2_INSTALL_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${TINYXML2_INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=OFF
    )

find_library(tinyxml2_LIBRARY
    NAMES libtinyxml2. tinyxml2
    HINTS "${TINYXML2_INSTALL_DIR}/lib" "${TINYXML2_INSTALL_DIR}/lib64"
    )

find_path(tinyxml2_INCLUDE_DIR
    NAMES include/tinyxml2.h
    HINTS ${TINYXML2_INSTALL_DIR}
    )

find_path(tinyxml2_DIR_PATH
        NAMES cmake/tinyxml2/tinyxml2Config.cmake
        PATHS ${TINYXML2_INSTALL_DIR}
        PATH_SUFFIXES lib64 lib
        )

set(tinyxml2_DIR "${tinyxml2_DIR_PATH}/cmake/tinyxml2/" CACHE INTERNAL "tinyxml2 include dir" FORCE)
set(tinyxml2_INCLUDE_DIRS "${tinyxml2_INCLUDE_DIR}/include" CACHE INTERNAL "tinyxml2 include dir" FORCE)
set(tinyxml2_LIBRARIES "tinyxml2::tinyxml2" CACHE INTERNAL "tinyxml2 library" FORCE)
set(tinyxml2_LIBRARY_DIR "${TINYXML2_INSTALL_DIR}/lib" CACHE INTERNAL "tinyxml2 library dir" FORCE)
