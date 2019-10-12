#ifndef MDFSORTER_MDF_BLOCK_H
#define MDFSORTER_MDF_BLOCK_H

#include "MDF_Header.h"

#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mdf {

    // Forward declare all blocks.
    struct CC_Block;
    struct CG_Block;
    struct CN_Block;
    struct DG_Block;
    struct DT_Block;
    struct FH_Block;
    struct ID_Block;
    struct HD_Block;
    struct MD_Block;
    struct SD_Block;
    struct SI_Block;
    struct TX_Block;

    struct BlockTree;

    struct MDF_Block : std::enable_shared_from_this<MDF_Block> {
        MDF_Block();
        MDF_Block(MDF_Block const& value);

        MDF_Header header;
        uint64_t fileLocation;

        virtual std::string toString() const;
        virtual bool load(std::istream& stream);
        virtual void save(std::ostream& stream);
        virtual void postLoad();

        std::vector<std::shared_ptr<MDF_Block>> blockLinks;
    protected:
        template<typename T>
        std::shared_ptr<T> createCopyIfSet(std::shared_ptr<T> const& value) const {
            std::shared_ptr<T> result;

            if(value) {
                result = std::make_shared<T>(*value);
            }

            return result;
        }

        uint64_t setLinkIfValidPointer(std::shared_ptr<MDF_Block> const& value) const {
            uint64_t result = 0;

            if(value) {
                result = value->fileLocation;
            }

            return result;
        }
    };

}

#endif //MDFSORTER_MDF_BLOCK_H
