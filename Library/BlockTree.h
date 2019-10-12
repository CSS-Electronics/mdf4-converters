#ifndef MDFSORTER_BLOCKTREE_H
#define MDFSORTER_BLOCKTREE_H

#include "Utilities/Tree.h"

#include <memory>
#include <set>
#include <vector>

namespace mdf {

    // Forward declaration of MDF_Block.
    struct MDF_Block;

    struct CC_Block;
    struct CG_Block;
    struct CN_Block;
    struct DG_Block;
    struct DT_Block;
    struct FH_Block;
    struct HD_Block;
    struct ID_Block;
    struct MD_Block;
    struct SI_Block;
    struct SD_Block;
    struct TX_Block;

    struct BlockTree : public Tree<std::shared_ptr<MDF_Block>> {
        BlockTree(std::shared_ptr<ID_Block> id, std::shared_ptr<HD_Block> hd);

        [[nodiscard]] std::string toString() const;
        std::set<std::shared_ptr<MDF_Block>> uniques;
    private:
        void walkNode(std::shared_ptr<MDF_Block> const& block);
        void walkNodeCC(std::shared_ptr<CC_Block> const& block);
        void walkNodeCN(std::shared_ptr<CN_Block> const& block);
        void walkNodeCG(std::shared_ptr<CG_Block> const& block);
        void walkNodeDG(std::shared_ptr<DG_Block> const& block);
        void walkNodeDT(std::shared_ptr<DT_Block> const& block);
        void walkNodeFH(std::shared_ptr<FH_Block> const& block);
        void walkNodeHD(std::shared_ptr<HD_Block> const& block);
        void walkNodeMD(std::shared_ptr<MD_Block> const& block);
        void walkNodeSD(std::shared_ptr<SD_Block> const& block);
        void walkNodeSI(std::shared_ptr<SI_Block> const& block);
        void walkNodeTX(std::shared_ptr<TX_Block> const& block);

        std::shared_ptr<MDF_Block> peek();
        void push(std::shared_ptr<MDF_Block> const& block);
        void pop();

        std::vector<std::shared_ptr<MDF_Block>> stack;
    };

}

#endif //MDFSORTER_BLOCKTREE_H
