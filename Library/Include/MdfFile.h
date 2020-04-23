#ifndef MDFSIMPLECONVERTERS_MDFFILE_H
#define MDFSIMPLECONVERTERS_MDFFILE_H

#include <string>

#include "CANRecord.h"
#include "FileInfo.h"
#include "LINRecord.h"
#include "RecordIterator.h"

namespace mdf {

  struct MdfFile {
    static std::unique_ptr<MdfFile> Create(std::string fileName);
    virtual ~MdfFile();
    virtual bool loadFileInfo() = 0;
    virtual bool finalize() = 0;
    virtual bool sort() = 0;
    virtual bool save(std::string fileName) = 0;
    [[nodiscard]] virtual FileInfo getFileInfo() const = 0;
    virtual RecordIterator<CANRecord const> getCANIterator() = 0;
    virtual RecordIterator<LINRecord const> getLINIterator() = 0;
  };

}

#endif //MDFSIMPLECONVERTERS_MDFFILE_H
