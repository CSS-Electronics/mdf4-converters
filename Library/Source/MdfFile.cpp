#include "MdfFile.h"
#include "MdfFileImplementation.h"

#include <fstream>
#include "CachingStreamBuffer.h"

namespace mdf {

    std::unique_ptr<MdfFile> MdfFile::Create(std::string fileName) {
        std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();

        do {
            // Create a new streambuf (For files), and open the target in binary read mode.
            std::shared_ptr<std::basic_filebuf<char>> streamParent = std::make_shared<std::basic_filebuf<char>>();
            auto ptr = streamParent->open(fileName, std::ios::in | std::ios::binary);

            std::shared_ptr<std::streambuf> stream = std::make_shared<CachingStreamBuffer>(streamParent, 1024*1024);

            if(ptr == nullptr) {
                break;
            }

            bool loadResult = result->load(stream);

            if (!loadResult) {
                result.reset();
            }
        } while(false);

        return result;
    }

    std::unique_ptr<MdfFile> MdfFile::Create(std::shared_ptr<std::istream> stream) {
        std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();

        return result;
    }

    std::unique_ptr<MdfFile> MdfFile::Create(std::shared_ptr<std::streambuf> stream) {
        std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();

        bool loadResult = result->load(stream);

        if (!loadResult) {
            result.reset();
        }

        return result;
    }

    MdfFile::~MdfFile() = default;

}
