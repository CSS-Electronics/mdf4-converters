#ifndef TOOLS_CSV_EXPORTER_H
#define TOOLS_CSV_EXPORTER_H

#include "ConverterInterface.h"

namespace mdf::tools::csv {

class CSV_Exporter : public mdf::tools::shared::ConverterInterface {
    public:
        CSV_Exporter();
        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
        [[nodiscard]] Version getVersion() const override;
    private:

    };

}

#endif //TOOLS_CSV_EXPORTER_H
