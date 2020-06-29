#ifndef MDFSIMPLECONVERTERS_RECORDFUNCTOR_H
#define MDFSIMPLECONVERTERS_RECORDFUNCTOR_H

#include <memory>

#include "../Blocks/IDataBlock.h"

namespace mdf {

    struct RecordFunctor {
        explicit RecordFunctor(std::shared_ptr<IDataBlock> storage);

        uint64_t operator()(std::size_t index) const;

        std::streamsize getData(char* buffer, std::streamsize n);

    protected:
        std::shared_ptr<IDataBlock> storage;
    };

}

#endif //MDFSIMPLECONVERTERS_RECORDFUNCTOR_H
