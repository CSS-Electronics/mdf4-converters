include(ExternalProject)
include(FetchContent)

set(EXTERNAL_PROJECT_NAME "PyCXX")

# Check for python 3 support.
find_package(Python3 COMPONENTS Interpreter Development)

if(Python3_Interpreter_FOUND AND Python3_Development_FOUND)
  # Python 3 development headers found, enable support for python in the module.
  set(PyCXX_ROOT ${EXTERNAL_PROJECT_INSTALL_DIR} CACHE PATH "PyCXX root path")
  set(ENV{PyCXX_ROOT} ${PyCXX_ROOT})
  find_package(PyCXX QUIET)

  message("External dependencies: Python found, searching for ${EXTERNAL_PROJECT_NAME}")
  if(NOT PyCXX_FOUND)
    message("External dependencies: ${EXTERNAL_PROJECT_NAME} not found, creating external target")

    # Command used for building.
    if(WIN32)
      set(EXTERNAL_PROJECT_BUILD_COMMAND
              COMMAND ${CMAKE_MAKE_PROGRAM} -f limited-api.mak clean
              )
    else()
      set(EXTERNAL_PROJECT_BUILD_COMMAND
              COMMAND ${CMAKE_MAKE_PROGRAM} -f limited-api.mak clean
              COMMAND ${CMAKE_MAKE_PROGRAM} -f limited-api.mak test
              )
    endif()


    message(${CMAKE_C_COMPILER})
    message(${CMAKE_CXX_COMPILER})

    # Command used for configuring the build.
    set(EXTERNAL_PROJECT_CONFIGURE_COMMAND
            COMMAND Python3::Interpreter <SOURCE_DIR>/setup_makefile.py linux limited-api.mak --limited-api=0x03040000
            # The configured file does not respect the CC and CXX environment variables.
            COMMAND sed -i s@gcc@${CMAKE_C_COMPILER}@g <SOURCE_DIR>/limited-api.mak
            COMMAND sed -i s@g++@${CMAKE_CXX_COMPILER}@g <SOURCE_DIR>/limited-api.mak
            #<SOURCE_DIR>/build-limited-api.sh ${Python3_EXECUTABLE} ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}
            )

    # Command used for installation.
    set(EXTERNAL_PROJECT_INSTALL_COMMAND
      # Copy headers.
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Config.hxx                          <INSTALL_DIR>/include/CXX/Config.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/CxxDebug.hxx                        <INSTALL_DIR>/include/CXX/CxxDebug.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Exception.hxx                       <INSTALL_DIR>/include/CXX/Exception.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Extensions.hxx                      <INSTALL_DIR>/include/CXX/Extensions.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/IndirectPythonInterface.hxx         <INSTALL_DIR>/include/CXX/IndirectPythonInterface.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Objects.hxx                         <INSTALL_DIR>/include/CXX/Objects.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Version.hxx                         <INSTALL_DIR>/include/CXX/Version.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/WrapPython.h                        <INSTALL_DIR>/include/CXX/WrapPython.h

      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/Config.hxx                  <INSTALL_DIR>/include/CXX/Python3/Config.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/cxx_standard_exceptions.hxx <INSTALL_DIR>/include/CXX/Python3/cxx_standard_exceptions.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/CxxDebug.hxx                <INSTALL_DIR>/include/CXX/Python3/CxxDebug.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/Exception.hxx               <INSTALL_DIR>/include/CXX/Python3/Exception.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/ExtensionModule.hxx         <INSTALL_DIR>/include/CXX/Python3/ExtensionModule.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/ExtensionOldType.hxx        <INSTALL_DIR>/include/CXX/Python3/ExtensionOldType.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/Extensions.hxx              <INSTALL_DIR>/include/CXX/Python3/Extensions.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/ExtensionType.hxx           <INSTALL_DIR>/include/CXX/Python3/ExtensionType.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/ExtensionTypeBase.hxx       <INSTALL_DIR>/include/CXX/Python3/ExtensionTypeBase.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/IndirectPythonInterface.hxx <INSTALL_DIR>/include/CXX/Python3/IndirectPythonInterface.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/Objects.hxx                 <INSTALL_DIR>/include/CXX/Python3/Objects.hxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/CXX/Python3/PythonType.hxx              <INSTALL_DIR>/include/CXX/Python3/PythonType.hxx

      # Copy sources.
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/cxx_exceptions.cxx                  <INSTALL_DIR>/src/PyCXX/Src/cxx_exceptions.cxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/cxx_extensions.cxx                  <INSTALL_DIR>/src/PyCXX/Src/cxx_extensions.cxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/cxxextensions.c                     <INSTALL_DIR>/src/PyCXX/Src/cxxextensions.c
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/cxxsupport.cxx                      <INSTALL_DIR>/src/PyCXX/Src/cxxsupport.cxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/IndirectPythonInterface.cxx         <INSTALL_DIR>/src/PyCXX/Src/IndirectPythonInterface.cxx

      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/Python3/cxx_exceptions.cxx          <INSTALL_DIR>/src/PyCXX/Src/Python3/cxx_exceptions.cxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/Python3/cxx_extensions.cxx          <INSTALL_DIR>/src/PyCXX/Src/Python3/cxx_extensions.cxx
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/Python3/cxxextensions.c             <INSTALL_DIR>/src/PyCXX/Src/Python3/cxxextensions.c
      COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/Src/Python3/cxxsupport.cxx              <INSTALL_DIR>/src/PyCXX/Src/Python3/cxxsupport.cxx
      )

    ExternalProject_Add (
      ${EXTERNAL_PROJECT_NAME}_builder

      PREFIX            ${EXTERNAL_PROJECT_NAME}
      URL               https://downloads.sourceforge.net/project/cxx/CXX/PyCXX%20V7.1.3/pycxx-7.1.3.tar.gz
      URL_HASH          MD5=0b2e64973c55a2e8dce5b4b8612bcb36

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
    message("External dependencies: ${EXTERNAL_PROJECT_NAME} found")
  endif()

endif()
