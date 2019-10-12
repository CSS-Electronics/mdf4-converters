project(mdf)

include_directories(Blocks)
include_directories(Include)
include_directories(Interfaces)
include_directories(Interfaces/Exceptions)

set(MDF_SOURCES
    BlockTree.cpp
    BlockTree.h
    ExtractMetaData.cpp
    ExtractMetaData.h
    Library.cpp
    Library.h
    Loader.cpp
    Loader.h
    MdfFile.cpp
    MdfFile.h
    SemanticVersion.cpp

    Blocks/CC_Block.cpp
    Blocks/CC_Block.h
    Blocks/CG_Block.cpp
    Blocks/CG_Block.h
    Blocks/CN_Block.cpp
    Blocks/CN_Block.h
    Blocks/DG_Block.cpp
    Blocks/DG_Block.h
    Blocks/DT_Block.cpp
    Blocks/DT_Block.h
        Blocks/FH_Block.cpp
    Blocks/FH_Block.h
    Blocks/HD_Block.cpp
    Blocks/HD_Block.h
    Blocks/ID_Block.cpp
    Blocks/ID_Block.h
    Blocks/MD_Block.cpp
    Blocks/MD_Block.h
    Blocks/SD_Block.cpp
    Blocks/SD_Block.h
    Blocks/SI_Block.cpp
    Blocks/SI_Block.h
    Blocks/TX_Block.cpp
    Blocks/TX_Block.h

    Config/VersionNumbers.h

    Include/CAN_DataFrame.h
    Include/FileInfo.h
    Include/LIN_Frame.h
    Include/MDF_File.h
    Include/SemanticVersion.h

    Interfaces/MDF_Block.cpp
    Interfaces/MDF_Block.h
    Interfaces/MDF_DataSource.cpp
    Interfaces/MDF_DataSource.h
    Interfaces/MDF_DataTypes.cpp
    Interfaces/MDF_DataTypes.h
    Interfaces/MDF_Header.cpp
    Interfaces/MDF_Header.h
    Interfaces/MDF_Type.cpp
    Interfaces/MDF_Type.h
    Interfaces/GenericDataReader.cpp
    Interfaces/GenericDataReader.h
    Interfaces/GenericDataRecord.cpp
    Interfaces/GenericDataRecord.h
    Interfaces/GenericDataRecordIterator.cpp
    Interfaces/GenericDataRecordIterator.h
    Interfaces/Exceptions/MDF_BlockSizeError.cpp
    Interfaces/Exceptions/MDF_BlockSizeError.h
    Interfaces/Exceptions/MDF_Exception.cpp
    Interfaces/Exceptions/MDF_Exception.h
    Interfaces/Exceptions/MDF_MissingBlockError.cpp
    Interfaces/Exceptions/MDF_MissingBlockError.h
    Interfaces/Exceptions/MDF_NotImplementedError.cpp
    Interfaces/Exceptions/MDF_NotImplementedError.h

    Iterators/CAN_DataFrame_Iterator.cpp
    Iterators/CAN_DataFrame_Iterator.h
    Iterators/CAN_DataFrame_Mapping.cpp
    Iterators/CAN_DataFrame_Mapping.h
    Iterators/LIN_Frame_Iterator.cpp
    Iterators/LIN_Frame_Iterator.h
    Iterators/LIN_Frame_Mapping.cpp
    Iterators/LIN_Frame_Mapping.h

    Utilities/Tree.cpp
    Utilities/Tree.h
    Utilities/TreeNode.cpp
    Utilities/TreeNode.h
    )

add_library(mdf STATIC ${MDF_SOURCES})
target_link_libraries(mdf interfaces ${tinyxml2_LIBRARIES})

target_include_directories(mdf INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Blocks>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Iterators>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/Exceptions>
    $<INSTALL_INTERFACE:include/mdf>
    )
