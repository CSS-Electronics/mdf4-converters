#include "MdfFile.h"
#include "MdfFileImplementation.h"

namespace mdf {

  std::unique_ptr<MdfFile> MdfFile::Create(std::string fileName) {
    std::unique_ptr<MdfFileImplementation> result = std::make_unique<MdfFileImplementation>();
    bool loadResult = result->load(fileName);

    if(!loadResult) {
      result.reset();
    }

    return result;
  }

  MdfFile::~MdfFile() = default;

}
