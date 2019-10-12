#include "Library.h"
#include "CAN_DataFrame.h"

#include <algorithm>
#include <forward_list>
#include <map>
#include <Exceptions/MDF_MissingBlockError.h>

#include "GenericDataRecordIterator.h"
#include "MDF_File.h"
#include "MdfFile.h"

#include "Version.h"

using namespace interfaces;

namespace mdf {

    constexpr interfaces::Version const version = {
        .major = 0,
        .minor = 2,
        .patch = 1
    };

    // TODO: Make re-entrant.
    std::map<MdfFile*, std::shared_ptr<CAN_DataFrame_Iterator>> loadedHandles;

    extern "C" void* mdf_openFile(char const* fileName) {
        MdfFile* result = nullptr;

        try {
            result = new MdfFile(fileName);
        } catch(MDF_MissingBlockError &e) {
            return nullptr;
        } catch(std::exception &e) {
            // Error during creation, most likely a non-existant file.
            return nullptr;
        }

        if(result != nullptr) {
            // Finalize and sort in memory.
            result->finalize();
            result->sort();

            std::shared_ptr<CAN_DataFrame_Iterator> iterator = result->getCANDataFrameIterator();

            // Extract handles.
            loadedHandles.emplace(result, iterator);
        }

        return static_cast<void*>(result);
    }

    extern "C" void mdf_closeFile(void* handle) {
        auto targetHandle = static_cast<MdfFile*>(handle);

        loadedHandles.erase(targetHandle);

        delete targetHandle;
    }

    extern "C" void mdf_save(void* handle, char const* fileName) {
        auto targetHandle = static_cast<MdfFile*>(handle);

        std::string fileNameStr(fileName);

        targetHandle->save(fileNameStr);
    }

    extern "C" bool mdf_getNextCANDataFrame(void* handle, CAN_DataFrame_t* record) {
        bool result = false;

        auto targetHandle = static_cast<MdfFile*>(handle);

        auto it = loadedHandles.find(targetHandle);

        if(it != loadedHandles.end()) {
            std::shared_ptr<CAN_DataFrame_Iterator> iterator = it->second;
            CAN_DataFrame_Iterator& iter = *iterator.get();

            // Check against end.
            auto end = targetHandle->getCANDataFrameIteratorEnd();
            auto endIter = *end.get();

            if(iter != endIter) {
                std::shared_ptr<CAN_DataFrame_t> data = *iter;

                *record = *data.get();
                iter++;
                result = true;
            }
        }

        return result;
    }

}
