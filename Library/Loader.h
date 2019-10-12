#ifndef MDFSORTER_LOADER_H
#define MDFSORTER_LOADER_H

#include <istream>
#include <map>
#include <memory>

#include "MDF_Block.h"

namespace mdf {

    /**
     * @class Loader
     *
     */
    class Loader {
    public:
        explicit Loader(std::istream& stream);
        std::shared_ptr<MDF_Block> loadBlock(uint64_t location);
        std::map<uint64_t, std::shared_ptr<MDF_Block>> getCache();
    private:
        std::shared_ptr<MDF_Block> loadBlockMDF4(uint64_t location);
        std::shared_ptr<MDF_Block> loadBlockID();

        std::istream& stream;
        std::map<uint64_t, std::shared_ptr<MDF_Block>> cache;
    };

}

#endif //MDFSORTER_LOADER_H
