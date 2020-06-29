#include "GenericFile.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <array>

#include <boost/endian/arithmetic.hpp>
#include <boost/endian/conversion.hpp>

namespace mdf::generic {

    static constexpr size_t headerBytes = 20;
    static constexpr std::array<uint8_t, 12> magicBytes = {'G', 'e', 'n', 'e', 'r', 'i', 'c', ' ', 'F', 'i', 'l', 'e'};

    GenericFileType getGenericFileType(boost::filesystem::path const& filePath) {
        GenericFileType result = GenericFileType::Unknown;

        try {
            GenericFile genericFile(filePath);
            result = genericFile.getGenericFileType();
        } catch (std::exception &e) {

        }

        return result;
    }

    class GenericFile::GenericFileImplementation {
    public:
        explicit GenericFileImplementation(std::vector<uint8_t> const& data);

        uint32_t deviceID;
        uint8_t fileVersionMajor;
        uint8_t fileVersionMinor;
        uint8_t specificFileType;
        GenericFileType genericFileType;
    private:
    };

    GenericFile::GenericFile(std::istream &dataStream) {
        // Read the required number of bytes from the header.
        std::vector<uint8_t> data(headerBytes);
        std::back_insert_iterator dataIterator(data);
        std::istream_iterator<uint8_t> dataStreamIterator(dataStream);

        std::copy_n(dataStreamIterator, headerBytes, dataIterator);

        implementation = std::make_unique<GenericFileImplementation>(data);
    }

    GenericFile::GenericFile(std::vector<uint8_t> const &data) {
        implementation = std::make_unique<GenericFileImplementation>(data);
    }

    GenericFile::GenericFile(boost::filesystem::path const& filePath) {
        // Attempt to open the file.
        if(!boost::filesystem::exists(filePath)) {
            throw std::runtime_error("File does not exist");
        }

        // Open the file (Do not skip whitespaces).
        std::ifstream fileSource(filePath.string(), std::ios_base::in | std::ios_base::binary);
        std::noskipws(fileSource);
        std::istream_iterator<uint8_t> dataStreamIterator(fileSource);

        // Storage for the header data.
        std::vector<uint8_t> data(headerBytes);

        // Read the required number of bytes from the header.
        std::copy_n(dataStreamIterator, headerBytes, std::begin(data));

        implementation = std::make_unique<GenericFileImplementation>(data);
    }

    GenericFile::~GenericFile() = default;

    uint32_t GenericFile::getDeviceID() const {
        return implementation->deviceID;
    }

    uint8_t GenericFile::getFileVersionMajor() const {
        return implementation->fileVersionMajor;
    }

    uint8_t GenericFile::getFileVersionMinor() const {
        return implementation->fileVersionMinor;
    }

    GenericFileType GenericFile::getGenericFileType() const {
        return implementation->genericFileType;
    }

    GenericFile::GenericFileImplementation::GenericFileImplementation(std::vector<uint8_t> const &data) {
        // Ensure enough data is present to construct the implementation.
        if(data.size() < headerBytes) {
            throw std::runtime_error("Could not create GenericFileImplementation, since too few data bytes were present.");
        } else if(data.size() > headerBytes) {
            throw std::runtime_error("Could not create GenericFileImplementation, since too many data bytes were present.");
        }

        // Check the magic header.
        if(!std::equal(magicBytes.cbegin(), magicBytes.cend(), data.cbegin())) {
            throw std::runtime_error("Wrong magic header.");
        }

        // Read out data.
        fileVersionMajor = data[12];
        fileVersionMinor = data[13];

        genericFileType = GenericFileType::Unknown;

        switch(data[14]) {
            case static_cast<std::underlying_type<GenericFileType>::type>(GenericFileType::CompressedFile):
                genericFileType = GenericFileType::CompressedFile;
                break;
            case static_cast<std::underlying_type<GenericFileType>::type>(GenericFileType::EncryptedFile):
                genericFileType = GenericFileType::EncryptedFile;
                break;
            default:
                break;
        }

        specificFileType = data[15];

        // Read the device ID as little endian.
        deviceID = boost::endian::load_big_u32(data.data() + 16);
    }
}
