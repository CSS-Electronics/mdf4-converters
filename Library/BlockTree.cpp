#include "BlockTree.h"

#include "Blocks/CC_Block.h"
#include "Blocks/CN_Block.h"
#include "Blocks/CG_Block.h"
#include "Blocks/DG_Block.h"
#include "Blocks/DT_Block.h"
#include "Blocks/FH_Block.h"
#include "Blocks/HD_Block.h"
#include "Blocks/ID_Block.h"
#include "Blocks/MD_Block.h"
#include "Blocks/SD_Block.h"
#include "Blocks/SI_Block.h"
#include "Blocks/TX_Block.h"

#include <sstream>

namespace mdf {

    BlockTree::BlockTree(std::shared_ptr<mdf::ID_Block> id, std::shared_ptr<mdf::HD_Block> hd) {
        // Start a new tree with ID at the root and HD as the first and only child.
        insertRoot(id);
        uniques.insert(id);
        push(id);

        // Begin walking all linked nodes.
        walkNode(hd);

        pop();
    }

    std::string BlockTree::toString() const {
        // Walk the tree.
        int counter = 0;
        std::stringstream s;

        for(auto iter = cbegin(); iter != cend(); ++iter) {
            int depth = iter->first;
            for(int i = 0; i < depth; ++i) {
                s << "  ";
            }

            s << iter->second->toString() << std::endl;
            counter += 1;
        }

        s << std::endl;
        s << "In total: " << counter << " blocks." << std::endl;
        s << "Unique: " << uniques.size() << " blocks." << std::endl;

        return s.str();
    }

    std::shared_ptr<MDF_Block> BlockTree::peek() {
        return *stack.rbegin();
    }

    void BlockTree::push(const std::shared_ptr<mdf::MDF_Block> &block) {
        stack.push_back(block);
    }

    void BlockTree::pop() {
        stack.pop_back();
    }

    void BlockTree::walkNode(std::shared_ptr<mdf::MDF_Block> const& block) {
        uniques.insert(block);

        switch(block->header.type) {
            case MDF_Type_CC:
                walkNodeCC(std::dynamic_pointer_cast<CC_Block>(block));
                break;
            case MDF_Type_CG:
                walkNodeCG(std::dynamic_pointer_cast<CG_Block>(block));
                break;
            case MDF_Type_CN:
                walkNodeCN(std::dynamic_pointer_cast<CN_Block>(block));
                break;
            case MDF_Type_DG:
                walkNodeDG(std::dynamic_pointer_cast<DG_Block>(block));
                break;
            case MDF_Type_DT:
                walkNodeDT(std::dynamic_pointer_cast<DT_Block>(block));
                break;
            case MDF_Type_FH:
                walkNodeFH(std::dynamic_pointer_cast<FH_Block>(block));
                break;
            case MDF_Type_HD:
                walkNodeHD(std::dynamic_pointer_cast<HD_Block>(block));
                break;
            case MDF_Type_MD:
                walkNodeMD(std::dynamic_pointer_cast<MD_Block>(block));
                break;
            case MDF_Type_SI:
                walkNodeSI(std::dynamic_pointer_cast<SI_Block>(block));
                break;
            case MDF_Type_SD:
                walkNodeSD(std::dynamic_pointer_cast<SD_Block>(block));
                break;
            case MDF_Type_TX: {
                auto blockres = std::dynamic_pointer_cast<TX_Block>(block);
                walkNodeTX(std::dynamic_pointer_cast<TX_Block>(blockres));
            }
                break;
            default:
                break;
        }
    }

    void BlockTree::walkNodeCC(const std::shared_ptr<mdf::CC_Block> &block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Register the unit.
        MDF_Type unitType = block->getUnitType();
        if(unitType == MDF_Type_MD) {
            std::shared_ptr<MDF_Block> unit = block->getUnitMD();
            walkNode(unit);
        } else if(unitType == MDF_Type_TX) {
            std::shared_ptr<MDF_Block> unit = block->getUnitTX();
            walkNode(unit);
        }

        // Register name.
        std::shared_ptr<TX_Block> name = block->getName();
        if(name) {
            walkNode(name);
        }

        pop();
    }

    void BlockTree::walkNodeCG(std::shared_ptr<mdf::CG_Block> const& block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Register acquisition name.
        std::shared_ptr<TX_Block> acquisitionName = block->getAcquisitionName();
        if(acquisitionName) {
            walkNode(acquisitionName);
        }

        // Register acquisition source.
        std::shared_ptr<SI_Block> acquisitionSource = block->getAcquisitionSource();
        if(acquisitionSource) {
            walkNode(acquisitionSource);
        }

        // Walk all CN blocks.
        for(std::shared_ptr<CN_Block> const& cnBlock: block->cn) {
            walkNode(cnBlock);
        }

        pop();
    }

    void BlockTree::walkNodeCN(std::shared_ptr<mdf::CN_Block> const& block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Register the unit.
        std::shared_ptr<MD_Block> unit = block->getUnit();
        if(unit) {
            walkNode(unit);
        }

        // Register name.
        std::shared_ptr<TX_Block> name = block->getName();
        if(name) {
            walkNode(name);
        }

        // Register source information.
        std::shared_ptr<SI_Block> source = block->getSource();
        if(source) {
            walkNode(source);
        }

        // Register conversion.
        std::shared_ptr<CC_Block> conversion = block->getConversion();
        if(conversion) {
            walkNode(conversion);
        }

        // Register the data block if not VLSD.
        if(block->getDataType() == MDF_Type_SD) {
            walkNode(block->getDataSD());
        }

        // If this is a composite channel, push this block and walk the composite group.
        for(std::shared_ptr<CN_Block> const& compositeBlock: block->composite) {
            walkNode(compositeBlock);
        }

        pop();
    }

    void BlockTree::walkNodeDG(std::shared_ptr<mdf::DG_Block> const& block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Walk all CG blocks.
        for(std::shared_ptr<CG_Block> const& cgBlock: block->cg) {
            walkNode(cgBlock);
        }

        // Register the data block.
        std::shared_ptr<MDF_Block> data = block->getDataBlock();
        if(data) {
            walkNode(data);
        }

        pop();
    }

    void BlockTree::walkNodeDT(const std::shared_ptr<mdf::DT_Block> &block) {
        // Register this block.
        insertNode(block, peek());
    }

    void BlockTree::walkNodeFH(const std::shared_ptr<mdf::FH_Block> &block) {
        // Register this block.
        insertNode(block, peek());

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            push(block);
            walkNode(comment);
            pop();
        }
    }

    void BlockTree::walkNodeHD(std::shared_ptr<HD_Block> const& block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Register all FH blocks.
        for(std::shared_ptr<FH_Block> const& fhBlock: block->fh) {
            walkNode(fhBlock);
        }

        // Walk all DG blocks.
        for(std::shared_ptr<DG_Block> const& dgBlock: block->dg) {
            walkNode(dgBlock);
        }

        pop();
    }

    void BlockTree::walkNodeMD(const std::shared_ptr<mdf::MD_Block> &block) {
        // Register this block.
        insertNode(block, peek());
    }

    void BlockTree::walkNodeSD(const std::shared_ptr<mdf::SD_Block> &block) {
        // Register this block.
        insertNode(block, peek());
    }

    void BlockTree::walkNodeSI(const std::shared_ptr<mdf::SI_Block> &block) {
        // Register this block.
        insertNode(block, peek());
        push(block);

        // Register any comments.
        std::shared_ptr<MD_Block> comment = block->getComment();
        if(comment) {
            walkNode(comment);
        }

        // Register name.
        std::shared_ptr<TX_Block> name = block->getName();
        if(name) {
            walkNode(name);
        }

        // Register path.
        std::shared_ptr<TX_Block> path = block->getPath();
        if(path) {
            walkNode(path);
        }

        pop();
    }

    void BlockTree::walkNodeTX(const std::shared_ptr<mdf::TX_Block> &block) {
        // Register this block.
        insertNode(block, peek());
    }

}
