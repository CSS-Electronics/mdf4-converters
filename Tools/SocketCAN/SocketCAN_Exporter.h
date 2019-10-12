#ifndef TOOLS_SOCKETCAN_EXPORTER_H
#define TOOLS_SOCKETCAN_EXPORTER_H

#include "ConverterInterface.h"

namespace tools::socketcan {

class SocketCAN_Exporter : public tools::shared::ConverterInterface {
    public:
        SocketCAN_Exporter();
        bool convert(boost::filesystem::path inputFilePath, boost::filesystem::path outputFolder) override;
        [[nodiscard]] interfaces::Version getVersion() const override;
    private:

    };

}

#endif //TOOLS_SOCKETCAN_EXPORTER_H
