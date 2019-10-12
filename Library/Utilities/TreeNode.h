#ifndef MDFSORTER_TREENODE_H
#define MDFSORTER_TREENODE_H

#include <vector>

namespace mdf {

    template<typename T>
    struct TreeNode {
        TreeNode();
        explicit TreeNode(T);
        explicit TreeNode(T, int depth);

        T node;
        int depth;
        std::vector<TreeNode<T>> children;
    };

    template<typename T>
    TreeNode<T>::TreeNode() : TreeNode(T()) {
        //
    }

    template<typename T>
    TreeNode<T>::TreeNode(T value) : TreeNode(value, 0) {
        //
    }

    template<typename T>
    TreeNode<T>::TreeNode(T value, int depth) : node(value), depth(depth) {
        //
    }

}

#endif //MDFSORTER_TREENODE_H
