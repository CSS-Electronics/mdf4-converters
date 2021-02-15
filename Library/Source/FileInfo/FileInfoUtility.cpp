#include "FileInfoUtility.h"

namespace mdf {

    std::shared_ptr<CNBlock> getMasterTimeChannel(std::shared_ptr<CGBlock> cgBlock) {
        std::shared_ptr<CNBlock> cnBlock = cgBlock->getFirstCNBlock();

        while(cnBlock) {
            bool resultFound = false;

            do {
                if((cnBlock->getChannelType() & CNType::MasterChannel) != CNType::MasterChannel) {
                    break;
                }

                if((cnBlock->getSyncType() & CNSyncType::Time) != CNSyncType::Time) {
                    break;
                }

                resultFound = true;
            } while(false);

            if(resultFound) {
                break;
            }

            // NOTE: No need to check composition blocks, as they cannot contain the master channel.
            cnBlock = cnBlock->getNextCNBlock();
        }

        // If a result was located, cnBlock is a valid pointer to the master channel. Else, it has no reference.
        return cnBlock;
    }

}
