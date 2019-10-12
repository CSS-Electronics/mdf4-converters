#include "MDF_BlockSizeError.h"

#include <sstream>

namespace mdf {

    MDF_BlockSizeError MDF_BlockSizeError::Create(std::shared_ptr<mdf::MDF_Block> block, uint64_t finalLocation) {
        std::stringstream s;

        uint64_t expectedLocation = block->header.size + block->fileLocation;

        s << "Wrote over block boundary for block at " << block->fileLocation << "." << std::endl;
        s << "Expected size: " << block->header.size << ", expected end location: " << expectedLocation << std::endl;
        s << "Actual file end location: " << finalLocation << std::endl;
        s << "Difference: " << finalLocation - expectedLocation << std::endl;

        return MDF_BlockSizeError(s.str());
    }

    MDF_BlockSizeError::MDF_BlockSizeError(std::string const &what) : MDF_Exception(what) {
        //
    }

}
