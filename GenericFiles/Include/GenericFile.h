#ifndef MDFSIMPLECONVERTERS_GENERICFILE_H
#define MDFSIMPLECONVERTERS_GENERICFILE_H

#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

#include <boost/filesystem.hpp>

namespace mdf::generic {

    enum class GenericFileType : uint16_t {
        Unknown = 0x00,
        EncryptedFile = 0x11,
        CompressedFile = 0x22,
    };

    class GenericFile {
    protected:
        class GenericFileImplementation;
        std::unique_ptr<GenericFileImplementation> implementation;
    public:
        explicit GenericFile(std::vector<uint8_t> const& data);
        explicit GenericFile(std::istream& dataStream);
        explicit GenericFile(boost::filesystem::path const& filePath);
        ~GenericFile();

        [[nodiscard]] uint32_t getDeviceID() const;
        [[nodiscard]] uint8_t getFileVersionMajor() const;
        [[nodiscard]] uint8_t getFileVersionMinor() const;
        [[nodiscard]] GenericFileType getGenericFileType() const;
    };

    [[nodiscard]] GenericFileType getGenericFileType(boost::filesystem::path const& filePath);

}

#endif //MDFSIMPLECONVERTERS_GENERICFILE_H
