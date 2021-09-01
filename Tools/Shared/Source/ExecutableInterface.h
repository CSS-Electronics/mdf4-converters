#ifndef TOOLS_SHARED_EXECUTABLEINTERFACE_H
#define TOOLS_SHARED_EXECUTABLEINTERFACE_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "ConverterInterface.h"
#include "ParseOptionStatus.h"
#include "PasswordStorage.h"
#include "StatusCode.h"

namespace mdf::tools::shared {

  class ExecutableInterface {
  public:
    explicit ExecutableInterface(std::unique_ptr<ConverterInterface> interface);

    /**
     * Entry point.
     * @param argc Number of arguments passed on the commandline.
     * @param argv Values of arguments passed on the commandline.
     * @return Status.
     */
    StatusCode main(int argc, char** argv);

  private:
    /**
     * Configures the argument parser for shared properties.
     */
    void configureParser();

    /**
     * Used to print help text to the commandline.
     */
    void displayHelp() const;

    /**
     *
     * @param unrecognizedOptions
     */
    void displayUnrecognizedOptions(std::vector<std::string> const &unrecognizedOptions) const;

    /**
     * Prints version information to the commandline.
     */
    void displayVersion() const;

    /**
     *
     * @param result
     * @return
     */
    ParseOptionStatus parseOptions(boost::program_options::variables_map const &result);

    /**
     * List of shared and interface options to accept on the commandline.
     */
    boost::program_options::options_description commandlineOptions;

    /**
     * List of shared options to accept at specific locations.
     */
    boost::program_options::positional_options_description commandlinePositionalOptions;

    /**
     * List of interface options to accept in a configuration file.
     */
    boost::program_options::options_description configFileOptions;

    boost::program_options::variables_map optionResult;
    std::vector<boost::filesystem::path> inputFiles;
    bool deleteInputFiles;
    std::shared_ptr<CommonOptions> commonOptions;
    std::shared_ptr<PasswordStorage> passwordStorage;
    std::vector<std::string> earlyLogMessages;

    /**
     * Storage for the converter interface to utilize.
     */
    std::unique_ptr<ConverterInterface> interface;
  };

}

#endif //TOOLS_SHARED_EXECUTABLEINTERFACE_H
