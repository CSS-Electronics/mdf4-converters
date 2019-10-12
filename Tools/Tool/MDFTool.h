#ifndef TOOLS_MDFTOOL_H
#define TOOLS_MDFTOOL_H

#include "ConverterInterface.h"

#include <string>

namespace ts = tools::shared;

namespace tools::tool {

    class MDFTool : public ts::ConverterInterface {
    public:
        MDFTool();
        void configureParser(boost::program_options::options_description& opts) override;
        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
        ts::ParseOptionStatus parseOptions(boost::program_options::variables_map const& result) override;
        [[nodiscard]] interfaces::Version getVersion() const override;
    private:
        bool overwrite;
        bool sortOutput;
        std::string appendName;
    };

}

#endif //TOOLS_MDFTOOL_H
