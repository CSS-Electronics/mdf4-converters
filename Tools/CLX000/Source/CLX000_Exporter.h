#ifndef TOOLS_CLX_EXPORTER_H
#define TOOLS_CLX_EXPORTER_H

#include "ConverterInterface.h"

#include "CLX000_Configuration.h"

namespace ts = mdf::tools::shared;

namespace mdf::tools::clx {

    class CLX000_Exporter : public ts::ConverterInterface {
        public:
        CLX000_Exporter();
            bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
            void configureFileParser(boost::program_options::options_description& opts) override;
            [[nodiscard]] semver::version const& getVersion() const override;
            tools::shared::ParseOptionStatus parseOptions(boost::program_options::variables_map const& result) override;
        private:
            boost::program_options::options_description logOptions;
            boost::program_options::options_description dataFieldsOptions;
            CLX000_Configuration configuration;
        };

}

#endif //TOOLS_CLX_EXPORTER_H
