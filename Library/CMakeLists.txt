project(MDF_BusLogging_Library)

set(MDF_BusLogging_Library_VERSION_MAJOR 1)
set(MDF_BusLogging_Library_VERSION_MINOR 0)
set(MDF_BusLogging_Library_VERSION_PATCH 3)

# Search for Boost.
find_package(Boost REQUIRED COMPONENTS filesystem log)

# Improved formatting to string.
find_package(fmt REQUIRED)

find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp" REQUIRED)

# Embed information in the source code.
configure_file(
    Source/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Source/Version.h
    @ONLY
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

# List of all public header files.
set(INCLUDE_FILES
    Include/FileInfo.h
    Include/IIterator.h
    Include/MDFBusLoggingLibrary.h
    Include/MdfFile.h
    Include/Metadata.h
    Include/RecordIterator.h

    Include/CAN_DataFrame.h
    Include/CAN_ErrorFrame.h
    Include/CAN_RemoteFrame.h
    Include/LIN_ChecksumError.h
    Include/LIN_Frame.h
    Include/LIN_SyncError.h
    Include/LIN_TransmissionError.h
    Include/LIN_ReceiveError.h
    )

include_directories(Include)

# List of all core headers.
set(SOURCE_HEADERS
    Source/BlockStorage.h
    Source/IBlockStorage.h
    Source/CachingStreamBuffer.h
    Source/ExtractMetaData.h
    Source/Logger.h
    Source/MdfFileImplementation.h
    Source/StructAdaptors.h
    Source/UnfinalizedFileInfo.h
    Source/Utility.h

    Source/Blocks/CCBlock.h
    Source/Blocks/CGBlock.h
    Source/Blocks/CNBlock.h
    Source/Blocks/DGBlock.h
    Source/Blocks/DTBlockMultipleRecordIDs.h
    Source/Blocks/DTBlockRaw.h
    Source/Blocks/DTBlockSingleDiscontinuous.h
    Source/Blocks/DTBlockSingleContinuous.h
    Source/Blocks/DTBlock.h
    Source/Blocks/FHBlock.h
    Source/Blocks/HDBlock.h
    Source/Blocks/IDBlock.h
    Source/Blocks/IDataBlock.h
    Source/Blocks/MDBlock.h
    Source/Blocks/MdfBlockTypes.h
    Source/Blocks/MdfBlock.h
    Source/Blocks/MdfHeader.h
    Source/Blocks/SDBlock.h
    Source/Blocks/SDBlockContinuous.h
    Source/Blocks/SDBlockDiscontinuous.h
    Source/Blocks/SIBlock.h
    Source/Blocks/TXBlock.h

    Source/Iterators/Assign.h
    Source/Iterators/EmptyIterator.h
    Source/Iterators/GenericIterator.h
    Source/Iterators/MappingInformation.h
    Source/Iterators/RecordFunctor.h
    )

# List of all core source files.
set(SOURCE_FILES
    Source/BlockStorage.cpp
    Source/CachingStreamBuffer.cpp
    Source/ExtractMetaData.cpp
    Source/FileInfo.cpp
    Source/MDFBusLoggingLibrary.cpp
    Source/Logger.cpp
    Source/MdfFile.cpp
    Source/MdfFileImplementation.cpp
    Source/UnfinalizedFileInfo.cpp
    Source/Utility.cpp

    Source/Blocks/CCBlock.cpp
    Source/Blocks/CGBlock.cpp
    Source/Blocks/CNBlock.cpp
    Source/Blocks/DGBlock.cpp
    Source/Blocks/DTBlock.cpp
    Source/Blocks/DTBlockMultipleRecordIDs.cpp
    Source/Blocks/DTBlockRaw.cpp
    Source/Blocks/DTBlockSingleDiscontinuous.cpp
    Source/Blocks/DTBlockSingleContinuous.cpp
    Source/Blocks/FHBlock.cpp
    Source/Blocks/HDBlock.cpp
    Source/Blocks/IDBlock.cpp
    Source/Blocks/MDBlock.cpp
    Source/Blocks/MdfBlock.cpp
    Source/Blocks/SDBlock.cpp
    Source/Blocks/SDBlockContinuous.cpp
    Source/Blocks/SDBlockDiscontinuous.cpp
    Source/Blocks/SIBlock.cpp
    Source/Blocks/TXBlock.cpp

    Source/Iterators/EmptyIterator.cpp
    Source/Iterators/GenericIterator.cpp
    Source/Iterators/RecordIterator.cpp
    Source/Iterators/RecordFunctor.cpp

    Source/Records/CAN_DataFrame.cpp
    Source/Records/CAN_ErrorFrame.cpp
    Source/Records/CAN_RemoteFrame.cpp
    Source/Records/LIN_ChecksumError.cpp
    Source/Records/LIN_Frame.cpp
    Source/Records/LIN_ReceiveError.cpp
    Source/Records/LIN_SyncError.cpp
    Source/Records/LIN_TransmissionError.cpp
    Source/Records/RecordTypes.cpp
    )

# Create an amalgamated list for sources and target libraries depending on the options.
set(SOURCE_FINAL
    ${SOURCE_FILES}
    ${SOURCE_HEADER_FILES}
    ${INCLUDE_FILES}
    )

set(LIBRARIES_FINAL
    Boost::headers
    Boost::filesystem
    Boost::log
    fmt::fmt
    )

include(CMakePackageConfigHelpers)

# Create static library.
add_library(${PROJECT_NAME} STATIC ${SOURCE_FINAL})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES_FINAL})

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
target_include_directories(${PROJECT_NAME} PRIVATE
    ${NEARGYE_SEMVER_INCLUDE_DIRS}
    )
