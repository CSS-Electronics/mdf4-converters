#ifndef TOOLS_PEAK_CAN_EXPORTER_2_1_H
#define TOOLS_PEAK_CAN_EXPORTER_2_1_H

#include "PEAK_CAN_Exporter.h"

namespace mdf::tools::peak {

    class PEAK_CAN_Exporter_2_1 : public PEAK_CAN_Exporter {
    public:
        explicit PEAK_CAN_Exporter_2_1(std::ostream &output, FileInfo const &fileInfo);

        void correctHeader() override;

        void writeHeader() override;

        void writeRecord(PEAK_Record const &record) override;

    private:
        void write_CAN_DataFrame(mdf::CAN_DataFrame const &record);

        void write_CAN_RemoteFrame(mdf::CAN_RemoteFrame const &record);
    };

}

#endif //TOOLS_PEAK_CAN_EXPORTER_2_1_H
