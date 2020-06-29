#ifndef MDFSIMPLECONVERTERS_MDFBLOCK_H
#define MDFSIMPLECONVERTERS_MDFBLOCK_H

#include <memory>
#include <vector>

#include "MdfHeader.h"

namespace mdf {

    struct MdfBlock {
        MdfBlock() = default;
        MdfBlock(MdfBlock const& other) = default;
        virtual ~MdfBlock() = default;

        [[nodiscard]] MdfHeader const& getHeader() const;
        void setHeader(MdfHeader header);

        uint64_t getFileLocation() const;
        void setFileLocation(uint64_t fileLocation);

        [[nodiscard]] std::vector<std::shared_ptr<MdfBlock>> const& getLinks() const;
        bool save(std::streambuf *stream);
    protected:
        MdfHeader header;
        std::vector<std::shared_ptr<MdfBlock>> links;

        virtual bool load(std::shared_ptr<std::streambuf> stream) = 0;
        virtual bool saveBlockData(std::streambuf *stream) = 0;
    private:
        uint64_t fileLocation;

        friend std::shared_ptr<MdfBlock> createBlock(MdfHeader header, std::vector<std::shared_ptr<MdfBlock>> links, std::shared_ptr<std::streambuf> stream);
    };

}

#endif //MDFSIMPLECONVERTERS_MDFBLOCK_H
