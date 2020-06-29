#include "MdfBlock.h"

#include "CCBlock.h"
#include "CGBlock.h"
#include "CNBlock.h"
#include "DGBlock.h"
#include "DTBlockRaw.h"
#include "HDBlock.h"
#include "FHBlock.h"
#include "MDBlock.h"
#include "SDBlockContinuous.h"
#include "SIBlock.h"
#include "TXBlock.h"

#include <streambuf>
#include <iostream>
#include <boost/endian/buffers.hpp>
#include <boost/endian/conversion.hpp>

namespace mdf {

    void MdfBlock::setHeader(MdfHeader header_) {
        this->header = header_;
    }

    std::shared_ptr<MdfBlock> createBlock(MdfHeader header, std::vector<std::shared_ptr<MdfBlock>> links, std::shared_ptr<std::streambuf> stream) {
        std::shared_ptr<MdfBlock> result;

        // Determine which type to construct.
        switch(header.blockType) {
            case MdfBlockType_CC:
                result = std::make_shared<CCBlock>();
                break;
            case MdfBlockType_CG:
                result = std::make_shared<CGBlock>();
                break;
            case MdfBlockType_CN:
                result = std::make_shared<CNBlock>();
                break;
            case MdfBlockType_DG:
                result = std::make_shared<DGBlock>();
                break;
            case MdfBlockType_DT:
                result = std::make_shared<DTBlockRaw>();
                break;
            case MdfBlockType_FH:
                result = std::make_shared<FHBlock>();
                break;
            case MdfBlockType_HD:
                result = std::make_shared<HDBlock>();
                break;
            case MdfBlockType_MD:
                result = std::make_shared<MDBlock>();
                break;
            case MdfBlockType_SD:
                result = std::make_shared<SDBlockContinuous>();
                break;
            case MdfBlockType_SI:
                result = std::make_shared<SIBlock>();
                break;
            case MdfBlockType_TX:
                result = std::make_shared<TXBlock>();
                break;
            default:
                break;
        }

        if(result) {
            // Set the original file location.
            std::streampos fileLocation = stream->pubseekoff(0, std::ios_base::cur);
            result->setFileLocation(fileLocation);

            // Set the header.
            result->header = header;

            // Set the block links.
            result->links = links;

            // Let the block perform the final loading.
            result->load(stream);
        }

        return result;
    }

    const MdfHeader & MdfBlock::getHeader() const {
        return header;
    }

    const std::vector<std::shared_ptr<MdfBlock> > & MdfBlock::getLinks() const {
        return links;
    }

    uint64_t MdfBlock::getFileLocation() const {
        return fileLocation;
    }

    void MdfBlock::setFileLocation(uint64_t fileLocation_) {
        fileLocation = fileLocation_;
    }

    bool MdfBlock::save(std::streambuf *stream) {
        // Save the header.
        std::streamsize written = stream->sputn(reinterpret_cast<const char *>(&header), sizeof(header));

        // Save the links.
        for(auto& link: links) {
            boost::endian::little_uint64_buf_t linkStorage(0);

            if(link) {
                // Extract the link address.
                linkStorage = link->getFileLocation();
            }

            written += stream->sputn(reinterpret_cast<const char *>(&linkStorage), sizeof(linkStorage));
        }

        // Delegate data save to block sub-class.
        return saveBlockData(stream);
    }

}
