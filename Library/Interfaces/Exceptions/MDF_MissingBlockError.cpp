#include "MDF_MissingBlockError.h"


#include <sstream>

namespace mdf {

    MDF_MissingBlockError MDF_MissingBlockError::Create(std::shared_ptr<mdf::MDF_Block> block,
                                                        std::string const& missingMember) {
        std::stringstream s;

        s << "Expected a block member for block at " << block->fileLocation << "." << std::endl;
        s << "Missing member is: " << missingMember << std::endl;

        return MDF_MissingBlockError(s.str());
    }

    MDF_MissingBlockError::MDF_MissingBlockError(std::string const &what) : MDF_Exception(what) {
        //
    }

}
