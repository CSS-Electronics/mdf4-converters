project(MDF_BusLogging_Library)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)

set(CMAKE_CXX_STANDARD 17)

# Search for Boost.
find_package(Boost REQUIRED COMPONENTS filesystem log)

# Improved formatting to string.
find_package(fmt REQUIRED)

# Support for reading XML.
find_package(tinyxml2 REQUIRED)

# Embed information in the source code.
configure_file(
    Source/ProjectInformation.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Source/ProjectInformation.h
    @ONLY
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Source)

# List of all public header files.
set(INCLUDE_FILES
    Include/CANRecord.h
    Include/FileInfo.h
    Include/IIterator.h
    Include/Library.h
    Include/LINRecord.h
    Include/MdfFile.h
    Include/RecordIterator.h
    Include/Version.h

    Include/spimpl.h
    )

include_directories(Include)

# List of all core headers.
set(SOURCE_HEADERS
    Source/BlockStorage.h
    Source/IBlockStorage.h
    Source/ExtractMetaData.h
    Source/MdfFileImplementation.h
    Source/StructAdaptors.h
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
    Source/Iterators/GenericIterator.h
    Source/Iterators/MappingInformation.h
    Source/Iterators/RecordFunctor.h

    Source/mio.h
    )

# List of all core source files.
set(SOURCE_FILES
    Source/BlockStorage.cpp
    Source/CANRecord.cpp
    Source/ExtractMetaData.cpp
    Source/FileInfo.cpp
    Source/Library.cpp
    Source/LINRecord.cpp
    Source/MdfFile.cpp
    Source/MdfFileImplementation.cpp
    Source/Utility.cpp
    Source/Version.cpp

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

    Source/Iterators/GenericIterator.cpp
    Source/Iterators/RecordIterator.cpp
    Source/Iterators/RecordFunctor.cpp
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
    tinyxml2::tinyxml2
    )

# Create static library.
add_library(${PROJECT_NAME}_static STATIC ${SOURCE_FINAL})
add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}_static)
target_link_libraries(${PROJECT_NAME}_static ${LIBRARIES_FINAL})

target_include_directories(${PROJECT_NAME}_static INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )

# Create shared library.
add_library(${PROJECT_NAME}_shared SHARED ${SOURCE_FINAL})
add_library(${PROJECT_NAME}::shared ALIAS ${PROJECT_NAME}_shared)
target_link_libraries(${PROJECT_NAME}_shared
    -static-libstdc++
    -static-libgcc
    -static
    ${LIBRARIES_FINAL}
    )

target_include_directories(${PROJECT_NAME}_shared INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    )
