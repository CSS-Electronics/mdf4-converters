#ifndef TOOLS_SHARED_CONVERTERINTERFACE_H
#define TOOLS_SHARED_CONVERTERINTERFACE_H

#include <ios>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include <CAN_DataFrame.h>

#include "CommonOptions.h"
#include "FileInfo.h"
#include "ProgressInterface.h"
#include "ParseOptionStatus.h"
#include "Version.h"

namespace tools::shared {

    class ConverterInterface : public progress::ProgressController {
    public:

        /**
         * Destructor.
         */
        virtual ~ConverterInterface();

        virtual bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) = 0;
        virtual void configureParser(boost::program_options::options_description& opts);
        virtual void configureFileParser(boost::program_options::options_description& opts);
        int setCommonOptions(std::shared_ptr<CommonOptions> commonOptions);
        virtual ParseOptionStatus parseOptions(boost::program_options::variables_map const& result);

        std::string const programName;
        [[nodiscard]] virtual interfaces::Version getVersion() const = 0;
        [[nodiscard]] bool usesConfigFile() const;
    protected:
        explicit ConverterInterface(std::string programName);
        std::shared_ptr<CommonOptions> commonOptions;
    private:
        bool readConfigurationFile = true;
    };

}

#endif //MDFSORTER_CONVERTERINTERFACE_H
