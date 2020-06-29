include(ExternalProject)
include(FetchContent)

# Set expected path for PyCXX.
find_package(PyCXX QUIET)

if(NOT PyCXX_FOUND)
    message(STATUS "Missing PyCXX: Generating target")

    # Command used for installation.
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
            # Copy all headers.
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Config.hxx                          <INSTALL_DIR>/include/CXX/Config.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/CxxDebug.hxx                        <INSTALL_DIR>/include/CXX/CxxDebug.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Exception.hxx                       <INSTALL_DIR>/include/CXX/Exception.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Extensions.hxx                      <INSTALL_DIR>/include/CXX/Extensions.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/IndirectPythonInterface.hxx         <INSTALL_DIR>/include/CXX/IndirectPythonInterface.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Objects.hxx                         <INSTALL_DIR>/include/CXX/Objects.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Version.hxx                         <INSTALL_DIR>/include/CXX/Version.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/WrapPython.h                        <INSTALL_DIR>/include/CXX/WrapPython.h

            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/Config.hxx                  <INSTALL_DIR>/include/CXX/Python3/Config.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/cxx_standard_exceptions.hxx <INSTALL_DIR>/include/CXX/Python3/cxx_standard_exceptions.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/CxxDebug.hxx                <INSTALL_DIR>/include/CXX/Python3/CxxDebug.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/Exception.hxx               <INSTALL_DIR>/include/CXX/Python3/Exception.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/ExtensionModule.hxx         <INSTALL_DIR>/include/CXX/Python3/ExtensionModule.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/ExtensionOldType.hxx        <INSTALL_DIR>/include/CXX/Python3/ExtensionOldType.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/Extensions.hxx              <INSTALL_DIR>/include/CXX/Python3/Extensions.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/ExtensionType.hxx           <INSTALL_DIR>/include/CXX/Python3/ExtensionType.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/ExtensionTypeBase.hxx       <INSTALL_DIR>/include/CXX/Python3/ExtensionTypeBase.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/IndirectPythonInterface.hxx <INSTALL_DIR>/include/CXX/Python3/IndirectPythonInterface.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/Objects.hxx                 <INSTALL_DIR>/include/CXX/Python3/Objects.hxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/CXX/Python3/PythonType.hxx              <INSTALL_DIR>/include/CXX/Python3/PythonType.hxx

            # Copy source files.
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/cxx_exceptions.cxx                  <INSTALL_DIR>/src/PyCXX/Src/cxx_exceptions.cxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/cxx_extensions.cxx                  <INSTALL_DIR>/src/PyCXX/Src/cxx_extensions.cxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/cxxextensions.c                     <INSTALL_DIR>/src/PyCXX/Src/cxxextensions.c
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/cxxsupport.cxx                      <INSTALL_DIR>/src/PyCXX/Src/cxxsupport.cxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/IndirectPythonInterface.cxx         <INSTALL_DIR>/src/PyCXX/Src/IndirectPythonInterface.cxx

            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/Python3/cxx_exceptions.cxx          <INSTALL_DIR>/src/PyCXX/Src/Python3/cxx_exceptions.cxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/Python3/cxx_extensions.cxx          <INSTALL_DIR>/src/PyCXX/Src/Python3/cxx_extensions.cxx
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/Python3/cxxextensions.c             <INSTALL_DIR>/src/PyCXX/Src/Python3/cxxextensions.c
            COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/Src/Python3/cxxsupport.cxx              <INSTALL_DIR>/src/PyCXX/Src/Python3/cxxsupport.cxx
            )

    ExternalProject_Add (
            PyCXX_fetch

            PREFIX            PyCXX
            URL               https://downloads.sourceforge.net/project/cxx/CXX/PyCXX%20V7.1.3/pycxx-7.1.3.tar.gz
            URL_HASH          MD5=0b2e64973c55a2e8dce5b4b8612bcb36

            CONFIGURE_COMMAND ""
            BUILD_COMMAND     ""
            INSTALL_COMMAND   ${EXTERNAL_PROJECT_INSTALL_COMMAND}
            UPDATE_COMMAND    ""

            BUILD_IN_SOURCE   1

            BUILD_ALWAYS      OFF
            INSTALL_DIR       ${PyCXX_ROOT}
    )
else()
    message(STATUS "PyCXX found")
endif()
