#ifndef MDFSIMPLECONVERTERS_MDFFILE_H
#define MDFSIMPLECONVERTERS_MDFFILE_H

#include <chrono>
#include <istream>
#include <string>

#include "FileInfo.h"
#include "Metadata.h"
#include "RecordIterator.h"

#include "CAN_DataFrame.h"
#include "CAN_ErrorFrame.h"
#include "CAN_RemoteFrame.h"
#include "LIN_ChecksumError.h"
#include "LIN_Frame.h"
#include "LIN_ReceiveError.h"
#include "LIN_SyncError.h"
#include "LIN_TransmissionError.h"

namespace mdf {

    struct MdfFile {
        static std::unique_ptr<MdfFile> Create(std::string fileName);

        static std::unique_ptr<MdfFile> Create(std::shared_ptr<std::istream> stream);

        static std::unique_ptr<MdfFile> Create(std::shared_ptr<std::streambuf> stream);

        virtual ~MdfFile();

        virtual bool finalize() = 0;

        virtual bool sort() = 0;

        virtual bool save(std::string fileName) = 0;

        virtual MetadataMap getMetadata() const = 0;

        virtual FileInfo getFileInfo() = 0;

        virtual std::chrono::nanoseconds getFirstMeasurement() = 0;

        virtual RecordIterator<CAN_DataFrame const> getCANIterator() = 0;

        virtual RecordIterator<CAN_ErrorFrame const> getCANErrorFrameIterator() = 0;

        virtual RecordIterator<CAN_RemoteFrame const> getCANRemoteFrameIterator() = 0;

        virtual RecordIterator<LIN_ChecksumError const> getLINChecksumErrorIterator() = 0;

        virtual RecordIterator<LIN_Frame const> getLINIterator() = 0;

        virtual RecordIterator<LIN_ReceiveError const> getLINReceiveErrorIterator() = 0;

        virtual RecordIterator<LIN_SyncError const> getLINSyncErrorIterator() = 0;

        virtual RecordIterator<LIN_TransmissionError const> getLINTransmissionErrorIterator() = 0;
    };

}

#endif //MDFSIMPLECONVERTERS_MDFFILE_H
