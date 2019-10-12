#ifndef MDFSORTER_MDFFILE_H
#define MDFSORTER_MDFFILE_H

#include <fstream>
#include <map>
#include <string>

#include "BlockTree.h"
#include "FileInfo.h"
#include "DG_Block.h"
#include "HD_Block.h"
#include "ID_Block.h"

#include "Iterators/CAN_DataFrame_Iterator.h"
#include "Iterators/LIN_Frame_Iterator.h"

namespace mdf {

    class MdfFile {
    public:
        explicit MdfFile(std::string const& fileName);

        void finalize();
        void sort();
        void save(std::string const& outputFileName);

        std::shared_ptr<CAN_DataFrame_Iterator> getCANDataFrameIterator();
        std::shared_ptr<CAN_DataFrame_Iterator> getCANDataFrameIteratorEnd();
        std::shared_ptr<LIN_Frame_Iterator> getLINFrameIterator();
        std::shared_ptr<LIN_Frame_Iterator> getLINFrameIteratorEnd();
        FileInfo_t getFileInfo() const;
    private:
        std::shared_ptr<ID_Block> ID;
        std::shared_ptr<HD_Block> HD;

        std::shared_ptr<BlockTree> tree;

        std::shared_ptr<CG_Block> findCANData() const;
        std::shared_ptr<CG_Block> findLINData() const;

        std::fstream inputStream;
        std::string const fileName;
    };

}

#endif //MDFSORTER_MDFFILE_H
