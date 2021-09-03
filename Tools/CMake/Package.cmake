function(pack_executable target)
    # Setup a dependency on all additional files, such that the target is rebuilt if any changes.
    setup_additional_files(${target})

    if (MINGW)
        pack_executable_mingw(${target})
    elseif (MSVC)
        pack_executable_msvc(${target})
    elseif (UNIX)
        pack_executable_unix(${target})
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

function(pack_executable_msvc target)
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

function(pack_executable_unix target)
    # Custom installer target to copy over the executable into the release directory.
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND $<$<CONFIG:Release>:${CMAKE_STRIP}> $<TARGET_FILE_NAME:${target}>
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${target}> ${CMAKE_BINARY_DIR}/Release/$<TARGET_FILE_NAME:${target}>
        )
endfunction()
