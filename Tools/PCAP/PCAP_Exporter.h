#ifndef TOOLS_PCAP_EXPORTER_H
#define TOOLS_PCAP_EXPORTER_H

#include "ConverterInterface.h"

namespace tools::pcap {

class PCAP_Exporter : public tools::shared::ConverterInterface {
    public:
        PCAP_Exporter();
        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
        [[nodiscard]] interfaces::Version getVersion() const override;
    private:

    };

}

#endif //TOOLS_PCAP_EXPORTER_H
