#ifndef MDFSIMPLECONVERTERS_PASSWORDSTORAGE_H
#define MDFSIMPLECONVERTERS_PASSWORDSTORAGE_H

#include <cstdint>
#include <exception>
#include <string>
#include <unordered_map>

#include <boost/filesystem.hpp>

namespace mdf::tools::shared {

    enum struct PasswordType {
        Default,
        Specific,
        Missing
    };

    class PasswordStorage {
    public:
        explicit PasswordStorage(boost::filesystem::path const &passwordFile);

        PasswordType hasPassword(uint32_t device) const;

        std::string const &getPassword(uint32_t device) const;

        boost::filesystem::path getPasswordFilePath();

    private:
        std::unordered_map<uint32_t, std::string> passwordMap;
        std::string defaultPassword;
        bool defaultPasswordSet;
        boost::filesystem::path passwordFilePath;
    };

    struct PasswordStorageException : std::exception {
        PasswordStorageException();
    };

}

#endif //MDFSIMPLECONVERTERS_PASSWORDSTORAGE_H
