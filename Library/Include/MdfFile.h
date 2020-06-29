#ifndef MDFSIMPLECONVERTERS_MDFFILE_H
#define MDFSIMPLECONVERTERS_MDFFILE_H

#include <chrono>
#include <istream>
#include <string>

#include "CANRecord.h"
#include "FileInfo.h"
#include "LINRecord.h"
#include "Metadata.h"
#include "RecordIterator.h"

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

        virtual RecordIterator<CANRecord const> getCANIterator() = 0;

        virtual RecordIterator<LINRecord const> getLINIterator() = 0;
    };

}

#endif //MDFSIMPLECONVERTERS_MDFFILE_H
