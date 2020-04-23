include(CMakePrintHelpers)
cmake_print_variables(APPIMAGE_DEP)

message("HELLO")
message("${APPIMAGE_DEP}")

# Find the appimage.
find_package(AppImage)

if(NOT AppImage_FOUND)
    message("Appimage not found, error")
endif()
