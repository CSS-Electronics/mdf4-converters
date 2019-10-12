#include "TX_Block.h"

#include <algorithm>
#include <sstream>

namespace mdf {

    static uint64_t constexpr size = sizeof(MDF_Header);
    static uint64_t constexpr linkCount = 0;

    //region Constructors

    TX_Block::TX_Block() {
        // Initialize header.
        header.type = MDF_Type_DG;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;
    }

    TX_Block::TX_Block(TX_Block const& value) : MDF_Block(value), stringData(value.stringData) {
        // Create a deep copy.
    }

    //endregion

    bool TX_Block::load(std::istream &stream) {
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

        if(stringSize > 1) {
            // Reserve space in advance to avoid re-allocations.
            stringData.reserve(stringSize);

            // Create an iterator for the input stream.
            auto iterator = std::istreambuf_iterator<char>(stream);

            // Copy over the data.
            std::copy_n(iterator, stringSize - 1, std::back_inserter(stringData));
        }

        return !stream.fail();
    }

    void TX_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Determine how many bytes to write (Add space for termination).
        uint64_t stringSize = stringData.size() + 1;

        // Write the data.
        stream << stringData;

        // Handle any alignment issues.
        auto alignmentError = stringSize % 8;

        if(alignmentError != 0) {
            uint64_t location = stream.tellp();
            location += 8 - alignmentError;
            stream.seekp(location);
        }
    }

    std::string TX_Block::getText() const {
        return stringData;
    }

}
