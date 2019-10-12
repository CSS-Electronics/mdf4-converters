#ifndef TOOLS_CSV_EXPORTER_H
#define TOOLS_CSV_EXPORTER_H

#include "ConverterInterface.h"

namespace tools::csv {

class CSV_Exporter : public tools::shared::ConverterInterface {
    public:
        CSV_Exporter();
        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
        [[nodiscard]] interfaces::Version getVersion() const override;
    private:

    };

}

#endif //TOOLS_CSV_EXPORTER_H
