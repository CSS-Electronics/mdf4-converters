function(pack_executable target)
    # Setup a dependency on all additional files, such that the target is rebuilt if any changes.
    setup_additional_files(${target})

    if (MINGW)
        pack_executable_mingw(${target})
    elseif (UNIX)
        pack_executable_appimage(${target})
    endif ()

    copy_additional_files(${target})
endfunction()

function(setup_additional_files target)
    get_property(ADDITIONAL_FILES TARGET ${target} PROPERTY ADDITIONAL_FILES)

    target_sources(${target} PRIVATE ${ADDITIONAL_FILES})
endfunction()

function(copy_additional_files target)
    # Copy the target and any additional files into the release folder.
    get_property(ADDITIONAL_FILES TARGET ${target} PROPERTY ADDITIONAL_FILES)

    foreach (ADDITIONAL_FILE ${ADDITIONAL_FILES})
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/${ADDITIONAL_FILE}
            ${CMAKE_BINARY_DIR}/Release/${target}_${ADDITIONAL_FILE}
            COPYONLY
        )
    endforeach ()
endfunction()

function(pack_executable_mingw target)
    # Insert the relevant data into the Windows resource file definition and place it in the binary directory.
    configure_file(
        ${RESOURCE_PATH}/Windows.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/Windows.rc
        @ONLY
    )

    # Add resource to executable source files.
    target_sources(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/Windows.rc)

    # Custom installer target to copy over the executable into the release directory.
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${target}> ${CMAKE_BINARY_DIR}/Release/$<TARGET_FILE_NAME:${target}>
    )
endfunction()

function(pack_executable_appimage target)
    configure_file(${RESOURCE_PATH}/AppImage.desktop.in AppImage.desktop @ONLY)
    set(BASE_PATH ${CMAKE_BINARY_DIR}/Release/${target})

    # If running in a docker container, the consensus on running appimages centers around extracting them first,
    # which may cause an issue with the cleanup process afterwards, if more than one process is used.
    # Commenting the "--appimage-extract-and-run" below will help in non-dockerized builds.
    set(INIT_COMMAND
        ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=${Boost_LIBRARY_DIR_RELEASE}"
        ${CMAKE_COMMAND} -E env "VERSION="
        ${CMAKE_SOURCE_DIR}/External/AppImage/src/appimage/linuxdeploy-x86_64.AppImage
        --appimage-extract-and-run
        --appdir ${BASE_PATH}
        --executable $<TARGET_FILE:${target}>
        --desktop-file ${CMAKE_CURRENT_BINARY_DIR}/AppImage.desktop
        --output appimage
        )

    set(COPY_COMMAND
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_PATH}/usr/share/icons/hicolor/16x16/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_PATH}/usr/share/icons/hicolor/32x32/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_PATH}/usr/share/icons/hicolor/64x64/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_PATH}/usr/share/icons/hicolor/128x128/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BASE_PATH}/usr/share/icons/hicolor/256x256/apps
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_PATH}/Icons/16x16.png ${BASE_PATH}/usr/share/icons/hicolor/16x16/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_PATH}/Icons/32x32.png ${BASE_PATH}/usr/share/icons/hicolor/32x32/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_PATH}/Icons/64x64.png ${BASE_PATH}/usr/share/icons/hicolor/64x64/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_PATH}/Icons/128x128.png ${BASE_PATH}/usr/share/icons/hicolor/128x128/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_PATH}/Icons/256x256.png ${BASE_PATH}/usr/share/icons/hicolor/256x256/apps/${target}.png
        )

    add_custom_target(${target}_install
        COMMAND ${COPY_COMMAND}

        # Pack using linuxdeploy.
        COMMAND ${INIT_COMMAND}

        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${target}>--x86_64.AppImage ${CMAKE_BINARY_DIR}/Release/$<TARGET_FILE_NAME:${target}>-x86_64.AppImage

        # Setup dependency chain.
        DEPENDS ${target}
        )
endfunction()
