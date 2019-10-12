function(pack_executable target)
    if(MINGW)
        pack_executable_mingw(${target})
    elseif(UNIX)
        pack_executable_appimage(${target})
    endif()

    copy_additional_files(${target})
endfunction()

function(copy_additional_files target)
    # Copy the target and any additional files into the release folder.
    get_property(ADDITIONAL_FILES TARGET ${target} PROPERTY ADDITIONAL_FILES)

    get_target_property(TARGET_TYPE ${target} TYPE)
    #message("Target ${target} is of type ${TARGET_TYPE}")

    if (${TARGET_TYPE} STREQUAL "EXECUTABLE")
        foreach(ADDITIONAL_FILE ${ADDITIONAL_FILES})
            add_custom_command(TARGET ${target}_install
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${ADDITIONAL_FILE} ${CMAKE_BINARY_DIR}/Release/${target}_${ADDITIONAL_FILE}
                COMMENT "Copying ${ADDITIONAL_FILE} for target ${target}"
                )
        endforeach()
    else()
        foreach(ADDITIONAL_FILE ${ADDITIONAL_FILES})
            add_custom_command(TARGET ${target}
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${ADDITIONAL_FILE} ${CMAKE_BINARY_DIR}/Release/${ADDITIONAL_FILE}
                COMMENT "Copying ${ADDITIONAL_FILE} for target ${target}"
                )
        endforeach()
    endif()
endfunction()

function(pack_executable_mingw target)
    # Insert the relevant data into the Windows resource file definition, and place it in the binary directory.
    configure_file(${CMAKE_SOURCE_DIR}/Resources/Windows.rc.in ${CMAKE_CURRENT_BINARY_DIR}/Windows.rc @ONLY)

    # Create a rule for compiling the Windows resource file.
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Windows.res
        COMMAND windres ${CMAKE_CURRENT_BINARY_DIR}/Windows.rc -O coff ${CMAKE_CURRENT_BINARY_DIR}/Windows.res
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Windows.rc
        COMMENT "Compiling Windows resource file"
        )

    # Bind the custom command into a target, to ensure it is present for the main target.
    add_custom_target(${target}_pre
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Windows.res
        SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Windows.res
        )
    add_dependencies(${target} ${target}_pre)

    # Ensure the created resource is linked into the executable target.
    set_property(TARGET ${target}
        APPEND
        PROPERTY LINK_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/Windows.res
        )

    # Custom installer target to copy over the executable into the release directory.
    add_custom_target(${target}_install
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${target}> ${CMAKE_BINARY_DIR}/Release/$<TARGET_FILE_NAME:${target}>

        # Setup dependency chain.
        DEPENDS ${target}
        )
endfunction()

function(pack_executable_appimage target)
    configure_file(${CMAKE_SOURCE_DIR}/Resources/AppImage.desktop.in AppImage.desktop @ONLY)
    set(BASE_PATH ${CMAKE_BINARY_DIR}/Release/${target})

    # If running in a docker container, the consensus on running appimages centers around extracting them first,
    # which may cause an issue with the cleanup process afterwards, if more than one process is used.
    # Commenting the "--appimage-extract-and-run" below will help in non-dockerized builds.
    set(INIT_COMMAND
        ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=${Boost_LIBRARY_DIR_RELEASE}"
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
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Resources/Icons/16x16.png ${BASE_PATH}/usr/share/icons/hicolor/16x16/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Resources/Icons/32x32.png ${BASE_PATH}/usr/share/icons/hicolor/32x32/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Resources/Icons/64x64.png ${BASE_PATH}/usr/share/icons/hicolor/64x64/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Resources/Icons/128x128.png ${BASE_PATH}/usr/share/icons/hicolor/128x128/apps/${target}.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Resources/Icons/256x256.png ${BASE_PATH}/usr/share/icons/hicolor/256x256/apps/${target}.png
        )

    add_custom_target(${target}_install
        COMMAND ${COPY_COMMAND}

        # Pack using linuxdeploy.
        COMMAND ${INIT_COMMAND}
		
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${target}>-x86_64.AppImage ${CMAKE_BINARY_DIR}/Release/$<TARGET_FILE_NAME:${target}>-x86_64.AppImage

        # Setup dependency chain.
        DEPENDS ${target}
        )
endfunction()