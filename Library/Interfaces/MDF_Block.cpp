#include "MDF_Block.h"

#include <sstream>

namespace mdf {

    MDF_Block::MDF_Block() : header() {
        fileLocation = INT32_MAX;
    }

    MDF_Block::MDF_Block(MDF_Block const& value) : enable_shared_from_this(value), header(value.header) {
        fileLocation = value.fileLocation;
    }

    std::string MDF_Block::toString() const {
        std::stringstream s;

        s << typeToString(header.type) << " - " << header.size << " - " << fileLocation << "(0x" << std::hex << fileLocation << std::dec << ")";

        return s.str();
    }

    bool MDF_Block::load(std::istream& stream) {
        // Load the header.
        auto* headerPtr = reinterpret_cast<char*>(&header);
        stream.read(headerPtr, sizeof(MDF_Header));

        return !stream.fail();
    }
    
    void MDF_Block::save(std::ostream &stream) {
        // Save the header.
        auto* headerPtr = reinterpret_cast<char*>(&header);
        stream.write(headerPtr, sizeof(MDF_Header));
    }

    void MDF_Block::postLoad() {
        //
    }

}
