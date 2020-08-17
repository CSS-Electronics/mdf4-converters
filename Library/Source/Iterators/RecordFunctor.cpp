#include "RecordFunctor.h"

#include <utility>

namespace mdf {

    RecordFunctor::RecordFunctor(std::shared_ptr<IDataBlock> storage) : storage(std::move(storage)) {
        //
    }

    uint64_t RecordFunctor::operator()(std::size_t index) const {
        return (*storage)[index];
    }

}
