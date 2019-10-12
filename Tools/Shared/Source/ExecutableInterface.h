#ifndef TOOLS_SHARED_EXECUTABLEINTERFACE_H
#define TOOLS_SHARED_EXECUTABLEINTERFACE_H

#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include "ConverterInterface.h"
#include "Library.h"
#include "ParseOptionStatus.h"

namespace tools::shared {

    class ExecutableInterface {
    public:
        explicit ExecutableInterface(std::unique_ptr<ConverterInterface> interface);
        int main(int argc, char** argv);
    private:
        boost::program_options::options_description commandlineOptions;
        boost::program_options::positional_options_description commandlinePositionalOptions;

        boost::program_options::options_description configFileOptions;

        boost::program_options::variables_map optionResult;
        std::vector<boost::filesystem::path> inputFiles;
        std::unique_ptr<ConverterInterface> interface;
        std::shared_ptr<CommonOptions> commonOptions;

        void configureParser();
        void displayHelp() const;
        void displayUnrecognizedOptions(std::vector<std::string> const& unrecognizedOptions) const;
        void displayVersion() const;
        ParseOptionStatus parseOptions(boost::program_options::variables_map const& result);
        void updateProgress(int, int);
    };

}

#endif //TOOLS_SHARED_EXECUTABLEINTERFACE_H
