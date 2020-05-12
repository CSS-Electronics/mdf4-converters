include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "AppImage")

set(AppImage_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "AppImage root path")
set(ENV{AppImage_ROOT} ${AppImage_ROOT})
find_package(AppImage)

if(NOT AppImage_FOUND)
    message("External dependencies: AppImage not found, creating external target.")

    # Command used for building.
    set(EXTERNAL_PROJECT_BUILD_COMMAND
        # Allow the executable to run.
        COMMAND chmod +x <DOWNLOAD_DIR>/linuxdeploy-x86_64.AppImage
        # Extract files for use within a container.
        COMMAND <DOWNLOAD_DIR>/linuxdeploy-x86_64.AppImage --appimage-extract
        )

    # Command used for installation.
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/opt
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/squashfs-root <INSTALL_DIR>/opt
        COMMAND ${CMAKE_COMMAND} -E copy <DOWNLOAD_DIR>/linuxdeploy-x86_64.AppImage <INSTALL_DIR>/opt/linuxdeploy-x86_64.AppImage
        COMMAND chmod +x <INSTALL_DIR>/opt/linuxdeploy-x86_64.AppImage
        )

    ExternalProject_Add (
        ${EXTERNAL_PROJECT_NAME}_builder

        PREFIX              ${EXTERNAL_PROJECT_NAME}
        URL                 https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
        URL_HASH            MD5=f245a3802a63e38cd53c54f2b2817e3e
        DOWNLOAD_NO_EXTRACT 1
        DOWNLOAD_NAME       linuxdeploy-x86_64.AppImage

        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ${EXTERNAL_PROJECT_BUILD_COMMAND}
        INSTALL_COMMAND     ${EXTERNAL_PROJECT_INSTALL_COMMAND}
        UPDATE_COMMAND      ""

        BUILD_IN_SOURCE     1

        BUILD_ALWAYS        OFF
        INSTALL_DIR         ${EXTERNAL_PROJECT_INSTALL_DIR}
    )

    # Add to targets list.
    list(APPEND EXTERNAL_TARGETS ${EXTERNAL_PROJECT_NAME}_builder)
    set(EXTERNAL_TARGETS ${EXTERNAL_TARGETS} PARENT_SCOPE)
else()
    message("External dependencies: AppImage packer (linuxdeploy) found at ${AppImage_INCLUDE_DIRS}")
endif()
