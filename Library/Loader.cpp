#include "Loader.h"

#include <iostream>
#include <stack>

#include "CC_Block.h"
#include "CG_Block.h"
#include "CN_Block.h"
#include "DG_Block.h"
#include "DT_Block.h"
#include "FH_Block.h"
#include "ID_Block.h"
#include "HD_Block.h"
#include "MD_Block.h"
#include "SD_Block.h"
#include "SI_Block.h"
#include "TX_Block.h"

#include "boost/range/irange.hpp"

namespace mdf {

    Loader::Loader(std::istream& stream) : stream(stream) {
        //
    }

    std::map<uint64_t, std::shared_ptr<MDF_Block>> Loader::getCache() {
        return cache;
    }

    std::shared_ptr<MDF_Block> Loader::loadBlock(uint64_t location) {
        std::shared_ptr<MDF_Block> result;

        if(location == 0) {
            result = loadBlockID();
        } else {
            result = loadBlockMDF4(location);
        }

        return result;
    }

    std::shared_ptr<MDF_Block> Loader::loadBlockID() {
        std::shared_ptr<MDF_Block> result;

        result = std::make_shared<ID_Block>();
        stream.seekg(0);
        if(!result->load(stream)) {
            result.reset();
        } else {
            cache.emplace(std::make_pair(0, result));
        }

        return result;
    }

    std::shared_ptr<MDF_Block> Loader::loadBlockMDF4(uint64_t location) {
        std::shared_ptr<MDF_Block> result;

        // Has a cached version already been loaded?
        auto iter = cache.find(location);

        if(iter != cache.end()) {
            result = iter->second;
            return result;
        }

        // Read the header at the specified location.
        MDF_Header header;
        auto* headerPtr = reinterpret_cast<char*>(&header);

        stream.seekg(location);
        stream.read(headerPtr, sizeof(MDF_Header));

        switch (header.type) {
            case MDF_Type_CC:
                result = std::make_shared<CC_Block>();
                break;
            case MDF_Type_CG:
                result = std::make_shared<CG_Block>();
                break;
            case MDF_Type_CN:
                result = std::make_shared<CN_Block>();
                break;
            case MDF_Type_DG:
                result = std::make_shared<DG_Block>();
                break;
            case MDF_Type_DT:
                result = std::make_shared<DT_Block>();
                break;
            case MDF_Type_FH:
                result = std::make_shared<FH_Block>();
                break;
            case MDF_Type_HD:
                result = std::make_shared<HD_Block>();
                break;
            case MDF_Type_MD:
                result = std::make_shared<MD_Block>();
                break;
            case MDF_Type_SD:
                result = std::make_shared<SD_Block>();
                break;
            case MDF_Type_SI:
                result = std::make_shared<SI_Block>();
                break;
            case MDF_Type_TX:
                result = std::make_shared<TX_Block>();
                break;
            default:
                break;
        }

        if(result) {
            // Load the block.
            result->fileLocation = location;
            stream.seekg(location);
            result->load(stream);

            // Cache the result.
            cache.emplace(std::make_pair(location, result));

            // Load all the link data.
            stream.seekg(result->fileLocation + sizeof(MDF_Header));
            std::vector<uint64_t> linkLocations(result->header.linkCount);

            //for(auto i: boost::irange(result->header.linkCount)) { // Disabled due to old boost version on raspbian
            uint64_t first = 0;
            for(auto i: boost::irange(first, result->header.linkCount)) {
                uint64_t linkLocation = 0;
                auto* linkLocationPtr = reinterpret_cast<char*>(&linkLocation);

                // Load the relative location.
                stream.read(linkLocationPtr, sizeof(linkLocation));

                linkLocations[i] = linkLocation;
            }

            // Perform proper loading of the links.
            result->blockLinks.clear();
            for(auto const& linkLocation: linkLocations) {
                std::shared_ptr<MDF_Block> linkResult;

                // Skip links which are not set.
                if(linkLocation != 0) {
                    linkResult = loadBlockMDF4(linkLocation);
                }

                // Store the result.
                result->blockLinks.push_back(linkResult);
            }

            // This block, and all its dependencies, has now been loaded. Perform post-load actions.
            result->postLoad();
        }

        return result;
    }
}
