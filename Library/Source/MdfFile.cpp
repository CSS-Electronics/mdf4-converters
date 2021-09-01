#include "MdfFile.h"
#include "MdfFileImplementation.h"

#include <fstream>
#include "CachingStreamBuffer.h"

#include "Streams/AESGCMStream.h"
#include "Streams/HeatshrinkStream.h"

namespace mdf {

    std::unique_ptr<MdfFile> MdfFile::Create(
            std::string const& fileName,
            std::optional<std::map<std::string, std::string>> const& passwords) {
        std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();

        do {
            // Create a new streambuf (For files), and open the target in binary read mode.
            std::unique_ptr<std::basic_filebuf<char>> streamParent = std::make_unique<std::basic_filebuf<char>>();
            auto ptr = streamParent->open(fileName, std::ios::in | std::ios::binary);

            if(ptr == nullptr) {
                result.reset();
                break;
            }

            auto passwordMap = passwords.value_or(std::map<std::string, std::string>());
            std::unique_ptr<std::streambuf> stream = std::move(streamParent);

            try {
                stream = mdf::stream::applyAESGCMFilter(std::move(stream), passwordMap);
                stream = mdf::stream::applyHeatshrinkFilter(std::move(stream));
            } catch(std::exception &e) {
                result.reset();
                break;
            }

            bool loadResult = result->load(std::move(stream));

            if (!loadResult) {
                result.reset();
            }
        } while(false);

        return result;
    }

    std::unique_ptr<MdfFile> MdfFile::Create(
            std::unique_ptr<std::streambuf> stream,
            std::optional<std::map<std::string, std::string>> const& passwords) {
        std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();

        do {

            auto passwordMap = passwords.value_or(std::map<std::string, std::string>());

            try {
                stream = mdf::stream::applyAESGCMFilter(std::move(stream), passwordMap);
                stream = mdf::stream::applyHeatshrinkFilter(std::move(stream));
            } catch (std::exception &e) {
                result.reset();
                break;
            }

            bool loadResult = result->load(std::move(stream));

            if (!loadResult) {
                result.reset();
            }
        } while(false);

        return result;
    }

    MdfFile::~MdfFile() = default;

}
