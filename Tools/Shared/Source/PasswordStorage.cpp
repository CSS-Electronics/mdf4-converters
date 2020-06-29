#include "PasswordStorage.h"

#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace mdf::tools::shared {

    PasswordStorageException::PasswordStorageException() = default;

    PasswordStorage::PasswordStorage(boost::filesystem::path const &passwordFile) {
        this->passwordMap.clear();
        this->defaultPassword = "";
        this->defaultPasswordSet = false;
        this->passwordFilePath = passwordFile;

        if (!boost::filesystem::exists(passwordFilePath)) {
            throw std::runtime_error("Password file not found");
        }

        std::ifstream inputStream(passwordFilePath.string(), std::ifstream::in);
        boost::property_tree::ptree tree;

        boost::property_tree::read_json(inputStream, tree);

        // Extract any device values.
        for (auto const &key: tree) {
            if (key.first == "default") {
                // Default key.
                this->defaultPassword = key.second.get_value<std::string>();
                this->defaultPasswordSet = true;
            } else if (key.first.length() == 8) {
                // 8 character long key, could be a device key. Attempt to parse as a hex value.
                try {
                    uint32_t mapKey = std::stoul(key.first, nullptr, 16);
                    this->passwordMap.emplace(mapKey, key.second.get_value<std::string>());
                } catch (std::invalid_argument &e) {
                    // Not a device key. Skip it.
                }
            }
        }
    }

    boost::filesystem::path PasswordStorage::getPasswordFilePath() {
        return passwordFilePath;
    }

    PasswordType PasswordStorage::hasPassword(uint32_t device) const {
        PasswordType result = PasswordType::Missing;

        auto iter = passwordMap.find(device);

        if (iter != std::end(passwordMap)) {
            result = PasswordType::Specific;
        } else if (defaultPasswordSet) {
            result = PasswordType::Default;
        }

        return result;
    }

    const std::string &PasswordStorage::getPassword(uint32_t device) const {
        // Perform lookup in the tree.
        auto iter = passwordMap.find(device);

        if (iter != std::end(passwordMap)) {
            return iter->second;
        } else if (defaultPasswordSet) {
            return defaultPassword;
        }

        return defaultPassword;
    }

}
