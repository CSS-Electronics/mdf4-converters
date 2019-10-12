#include "ConverterInterface.h"

namespace tools::shared {

    ConverterInterface::ConverterInterface(std::string programName) : programName(std::move(programName)) {

    }

    ConverterInterface::~ConverterInterface() = default;

    void ConverterInterface::configureParser(boost::program_options::options_description &opts) {

    }

    void ConverterInterface::configureFileParser(boost::program_options::options_description &opts) {
        readConfigurationFile = false;
    }

    ParseOptionStatus ConverterInterface::parseOptions(boost::program_options::variables_map const &result) {
        return ParseOptionStatus::NoError;
    }

    int ConverterInterface::setCommonOptions(std::shared_ptr<CommonOptions> commonOptions) {
        this->commonOptions = commonOptions;

        return 0;
    }

    bool ConverterInterface::usesConfigFile() const {
        return readConfigurationFile;
    }

}
