#include "DGBlock.h"

#include "DTBlockMultipleRecordIDs.h"
#include "DTBlockSingleDiscontinuous.h"
#include "TXBlock.h"
#include "../Utility.h"

#include <map>

#include <CAN_DataFrame.h>
#include "../Iterators/MappingInformation.h"
#include "DTBlockSingleContinuous.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

#pragma pack(push, 1)
    struct DGBlockData {
        be::little_uint8_buf_t dg_rec_id_size;
        be::little_uint8_buf_t reserved[7];
    };
#pragma pack(pop)

    constexpr MdfHeader DGBlockHeader(MdfBlockType_DG, 64, 4);

    DGBlock::DGBlock() {
        header = DGBlockHeader;
        recordIDSize = 0;

        for (uint64_t i = 0; i < DGBlockHeader.blockSize; ++i) {
            links.push_back({});
        }
    }

    std::shared_ptr<DGBlock> DGBlock::getNextDGBlock() const {
        return std::dynamic_pointer_cast<DGBlock>(links[0]);
    }

    std::shared_ptr<CGBlock> DGBlock::getFirstCGBlock() const {
        return std::dynamic_pointer_cast<CGBlock>(links[1]);
    }

    std::shared_ptr<MdfBlock> DGBlock::getDataBlock() const {
        return links[2];
    }

    void DGBlock::setNextDGBlock(std::shared_ptr<DGBlock> block) {
        links[0] = block;
    }

    void DGBlock::setFirstCGBlock(std::shared_ptr<CGBlock> block) {
        links[1] = block;
    }

    void DGBlock::setDataBlock(std::shared_ptr<MdfBlock> block) {
        links[2] = block;
    }

    bool DGBlock::load(std::shared_ptr<std::streambuf> stream) {
        bool result = false;

        // Load data into a struct for easier access.
        DGBlockData rawData;
        std::streamsize bytesRead = stream->sgetn(reinterpret_cast<char *>(&rawData), sizeof(rawData));

        if (bytesRead != sizeof(rawData)) {
            return false;
        }

        recordIDSize = rawData.dg_rec_id_size.value();

        // Create a list of record IDs and record sizes.
        std::shared_ptr<CGBlock> cgBlock = getFirstCGBlock();
        std::map<uint64_t, int64_t> recordSizes;

        while (cgBlock) {
            recordSizes.insert(std::make_pair(cgBlock->getRecordID(), cgBlock->getRecordSize()));

            cgBlock = cgBlock->getNextCGBlock();
        }

        auto originalDTBlock = std::dynamic_pointer_cast<DTBlock>(getDataBlock());
        std::shared_ptr<DTBlock> replacementDTBlock;

        // Create the correct type of DT block from the loaded DT block, depending on the IDs.
        if (recordSizes.size() > 1) {
            // Multiple IDs. Send the information to the DT block and create an index.
            auto currentDTBlock = std::make_shared<DTBlockMultipleRecordIDs>(*originalDTBlock);
            currentDTBlock->setRecordInformation(recordSizes, recordIDSize);
            currentDTBlock->index();

            replacementDTBlock = currentDTBlock;
        } else if (recordSizes.size() == 1) {
            // Extract record size from first CG block.
            std::shared_ptr<CGBlock> cgBlock = getFirstCGBlock();

            auto currentDTBlock = std::make_shared<DTBlockSingleContinuous>(*originalDTBlock, cgBlock->getRecordSize());

            replacementDTBlock = currentDTBlock;
        }

        setDataBlock(replacementDTBlock);

        result = true;

        return result;
    }

    void DGBlock::setRecordSize(uint8_t recordSize) {
        recordIDSize = recordSize;
    }

    uint8_t DGBlock::getRecordSize() const {
        return recordIDSize;
    }

    bool DGBlock::saveBlockData(std::streambuf *stream) {
        DGBlockData data;
        memset(&data, 0x00, sizeof(data));

        data.dg_rec_id_size = recordIDSize;

        std::streamsize written = stream->sputn(reinterpret_cast<const char *>(&data), sizeof(data));

        return (written == sizeof(data));
    }

}
