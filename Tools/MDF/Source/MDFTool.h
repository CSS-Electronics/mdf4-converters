#ifndef TOOLS_MDFTOOL_H
#define TOOLS_MDFTOOL_H

#include "ConverterInterface.h"

#include <string>

namespace mdf::tools::finalizer {

    class MDFTool : public mdf::tools::shared::ConverterInterface {
    public:
        MDFTool();

        void configureParser(boost::program_options::options_description &opts) override;

        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;

        mdf::tools::shared::ParseOptionStatus
        parseOptions(boost::program_options::variables_map const &result) override;

        [[nodiscard]] semver::version const& getVersion() const override;

    private:
        bool overwrite;
        std::string appendName;
    };

}

#endif //TOOLS_MDFTOOL_H
