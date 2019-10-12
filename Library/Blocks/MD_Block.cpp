#include "MD_Block.h"

#include <algorithm>
#include <sstream>

namespace mdf {

    MD_Block::MD_Block() {
        data = "";
    }

    MD_Block::MD_Block(std::string const& value) {
        data = value;
    }

    MD_Block::MD_Block(MD_Block const& value) : MDF_Block(value) {
        // Create a deep copy.
        data = value.data;
    }

    std::string MD_Block::getData() const {
        return data;
    }

    bool MD_Block::load(std::istream &stream) {
        // Let the base handle the initial loading.
        bool result = MDF_Block::load(stream);

        if(!result) {
            return result;
        }

        // Skip the links section.
        uint64_t currentLocation = stream.tellg();
        stream.seekg(currentLocation + header.linkCount * sizeof(uint64_t));

        // Load the data into a string.
        uint64_t stringSize = header.size - sizeof(header);

        if(stringSize > 0) {
            // Reserve space in advance to avoid re-allocations.
            data.reserve(stringSize);

            // Create an iterator for the input stream.
            auto iterator = std::istreambuf_iterator<char>(stream);

            // Copy over the data.
            std::copy_n(iterator, stringSize, std::back_inserter(data));
        }

        return !stream.fail();
    }

    void MD_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Determine how many bytes to write.
        uint64_t stringSize = data.size();

        // Write the data.
        stream << data;

        // Handle any alignment issues.
        auto alignmentError = stringSize % 8;

        if(alignmentError != 0) {
            uint64_t location = stream.tellp();
            location += 8 - alignmentError;
            stream.seekp(location);
        }
    }

}
