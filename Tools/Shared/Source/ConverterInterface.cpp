#include "ConverterInterface.h"

namespace mdf::tools::shared {

  ConverterInterface::ConverterInterface(std::string programName, bool usesConfigurationFile) :
    programNameData(std::move(programName)),
    readConfigurationFile(usesConfigurationFile) {
    this->programName = programNameData;
  }

  ConverterInterface::~ConverterInterface() = default;

  void ConverterInterface::configureParser(boost::program_options::options_description &opts) {
    // Default implementation does nothing.
  }

  void ConverterInterface::configureFileParser(boost::program_options::options_description &opts) {
    // Default implementation does nothing.
  }

  ParseOptionStatus ConverterInterface::parseOptions(boost::program_options::variables_map const &result) {
    return ParseOptionStatus::NoError;
  }

  int ConverterInterface::setCommonOptions(std::shared_ptr<CommonOptions> options) {
    commonOptions = std::move(options);

    return 0;
  }

  bool ConverterInterface::usesConfigFile() const {
    return readConfigurationFile;
  }

}
