#ifndef MDFSORTER_TREE_H
#define MDFSORTER_TREE_H

#include "TreeNode.h"

namespace mdf {

    template <typename T>
    struct Tree {
        Tree();
        Tree(Tree const& value);
        void insertRoot(T value);
        void insertNode(T value, T parent);

        typedef typename std::vector<std::pair<int, T>>::iterator iterator;
        typedef typename std::vector<std::pair<int, T>>::const_iterator const_iterator;

        iterator begin();
        const_iterator cbegin() const;

        iterator end();
        const_iterator cend() const;
        std::vector<std::pair<int, T>> flattened;
    private:
        TreeNode<T>* findNode(T& value, TreeNode<T>& parent);
        void flatten();
        void flatten(TreeNode<T> const& parent);

        TreeNode<T> rootNode;
    };

    template <typename T>
    Tree<T>::Tree() {
        rootNode = TreeNode<T>(T());
    }

    template <typename T>
    Tree<T>::Tree(Tree const& value) {
        rootNode = TreeNode<T>(T());
    }

    template <typename T>
    typename Tree<T>::iterator Tree<T>::begin() {
        return flattened.begin();
    }

    template <typename T>
    typename Tree<T>::const_iterator Tree<T>::cbegin() const {
        return flattened.cbegin();
    }

    template <typename T>
    typename Tree<T>::iterator Tree<T>::end() {
        return flattened.end();
    }

    template <typename T>
    typename Tree<T>::const_iterator Tree<T>::cend() const {
        return flattened.cend();
    }

    template <typename T>
    void Tree<T>::insertRoot(T value) {
        rootNode = TreeNode<T>(value);

        // Flatten the tree.
        flatten();
    }

    template <typename T>
    void Tree<T>::insertNode(T value, T parent) {
        // Attempt to find the parent node.
        TreeNode<T>* parentNode = findNode(parent, rootNode);

        if(parentNode != nullptr) {
            parentNode->children.emplace_back(value, parentNode->depth + 1);
        }

        // Flatten the tree.
        flatten();
    }

    template <typename T>
    void Tree<T>::flatten() {
        flattened.clear();

        flatten(rootNode);
    }

    template <typename T>
    void Tree<T>::flatten(TreeNode<T> const& parent) {
        // Insert this node.
        flattened.emplace_back(std::make_pair(parent.depth, parent.node));

        // Insert all children.
        for(auto const& child: parent.children) {
            flatten(child);
        }
    }

    template <typename T>
    TreeNode<T>* Tree<T>::findNode(T& value, TreeNode<T>& parent) {
        TreeNode<T>* result = nullptr;

        // Check if the parent matches.
        if(parent.node == value) {
            result = &parent;
        } else {
            // Look through the children.
            for(auto & child: parent.children) {
                result = findNode(value, child);

                if(result != nullptr) {
                    break;
                }
            }
        }

        return result;
    }
}

#endif //MDFSORTER_TREE_H
