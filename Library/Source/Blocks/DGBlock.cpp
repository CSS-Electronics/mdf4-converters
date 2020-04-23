#include "DGBlock.h"

#include "DTBlockMultipleRecordIDs.h"
#include "DTBlockSingleDiscontinuous.h"
#include "TXBlock.h"
#include "../Utility.h"

#include <map>

#include <boost/endian/buffers.hpp>
#include <boost/endian/conversion.hpp>
#include <CANRecord.h>
#include "../Iterators/MappingInformation.h"
#include "DTBlockSingleContinuous.h"

namespace mdf {

#pragma pack(push, 1)
  struct DGBlockData {
    boost::endian::little_uint8_buf_t dg_rec_id_size;
    boost::endian::little_uint8_buf_t reserved[7];
  };
#pragma pack(pop)

  constexpr MdfHeader DGBlockHeader = {
    .blockType = MdfBlockType_DG,
    .blockSize = 64,
    .linkCount = 4
  };

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

  bool DGBlock::load(uint8_t const *dataPtr) {
    bool result = false;

    // Load data into a struct for easier access.
    auto ptr = reinterpret_cast<DGBlockData const *>(dataPtr);

    recordIDSize = ptr->dg_rec_id_size.value();

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

  bool DGBlock::saveBlockData(uint8_t *dataPtr) {

    auto ptr = reinterpret_cast<DGBlockData *>(dataPtr);

    ptr->dg_rec_id_size = recordIDSize;

    return true;
  }

}
