# NOTE: Currently not used, kept for reference.

# Setup paths.
set(DATE_PREFIX_DIR ${EXTERNAL_PROJECTS_DIR}/date)
set(DATE_SOURCE_DIR ${DATE_PREFIX_DIR}/src/date)
set(DATE_INSTALL_DIR ${DATE_PREFIX_DIR}/root)

# Finally, gather all the commands
ExternalProject_Add(date_builder
        PREFIX ${DATE_PREFIX_DIR}
        URL https://github.com/HowardHinnant/date/archive/v2.4.1.tar.gz
        URL_MD5 "54d9dd9d5ac32214d3cbe8cc80bbccf2"
        SOURCE_DIR ${DATE_SOURCE_DIR}
        INSTALL_DIR ${DATE_INSTALL_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DATE_INSTALL_DIR} -DENABLE_DATE_TESTING=OFF -DHAS_REMOTE_API=0 -DAUTO_DOWNLOAD=0 -DUSE_TZ_DB_IN_DOT=ON
    )

find_library(date_LIBRARY
        NAMES libtz tz
        HINTS "${DATE_INSTALL_DIR}/lib" "${DATE_INSTALL_DIR}/lib64"
        )

find_path(date_LIBRARY_DIR_PATH
    NAMES libtz.a tz.a
    HINTS "${DATE_INSTALL_DIR}/lib" "${DATE_INSTALL_DIR}/lib64"
    )

find_path(date_INCLUDE_DIR
        NAMES include/date/tz.h
        HINTS ${DATE_INSTALL_DIR}
        )

find_path(date_DIR_PATH
        NAMES cmake/date/dateConfig.cmake CMake/dateConfig.cmake
        PATHS ${DATE_INSTALL_DIR}
        PATH_SUFFIXES lib64 lib
        )

set(date_DIR "${date_DIR_PATH}/cmake/date/" CACHE INTERNAL "date include dir" FORCE)
set(date_INCLUDE_DIRS "${date_INCLUDE_DIR}/include" CACHE INTERNAL "date include dir" FORCE)
set(date_LIBRARIES "date::date" CACHE INTERNAL "date library" FORCE)
set(date_LIBRARY_DIR "${date_LIBRARY_DIR_PATH}" CACHE INTERNAL "date library dir" FORCE)

