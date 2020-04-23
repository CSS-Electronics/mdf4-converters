#ifndef TOOLS_SHARED_CONVERTERINTERFACE_H
#define TOOLS_SHARED_CONVERTERINTERFACE_H

#include <ios>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include "CANRecord.h"

#include "CommonOptions.h"
#include "FileInfo.h"
//#include "ProgressInterface.h"
#include "ParseOptionStatus.h"
#include "Version.h"

namespace mdf::tools::shared {

  /**
   * The Converter interface allows each conversion tool to customize the commandline parser with additional parameters,
   * as well as use a default ini file for additional settings.
   */
  class ConverterInterface {// : public progress::ProgressController {
  public:

    /**
     * Destructor.
     */
    virtual ~ConverterInterface();

    virtual bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) = 0;

    /**
     * Called during the configuration stage of the commandline parser. Allows for injecting additional options.
     * @param opts
     */
    virtual void configureParser(boost::program_options::options_description &opts);

    /**
     * Called during the configuration stage of the configuration file parser. Allows for injecting additional options.
     * @param opts
     */
    virtual void configureFileParser(boost::program_options::options_description &opts);

    /**
     * Called after all options has been parsed, to allow the converter to respond to injected options.
     * @param result
     * @return
     */
    virtual ParseOptionStatus parseOptions(boost::program_options::variables_map const &result);

    int setCommonOptions(std::shared_ptr<CommonOptions> commonOptions);


    [[nodiscard]] virtual Version getVersion() const = 0;
    [[nodiscard]] bool usesConfigFile() const;

    /**
     * Get the name of the current convert interface.
     */
    std::string_view programName;

    /**
     * Is true if the interface uses a configuration file for additional settings.
     */
    bool const readConfigurationFile;

  protected:
    explicit ConverterInterface(std::string programName, bool usesConfigurationFile = false);

    std::shared_ptr<CommonOptions> commonOptions;
  private:
    std::string const programNameData;
  };

}

#endif //MDFSORTER_CONVERTERINTERFACE_H
