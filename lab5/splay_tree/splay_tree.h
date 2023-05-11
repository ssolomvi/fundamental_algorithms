#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include "../binary_tree/bs_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class splay_tree final
        : public bs_tree<tkey, tvalue, tkey_comparer>
{
public:
#pragma region template methods splay tree

    class template_method_splay final
            : public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
    public:
        void splay(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) const
        {
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent, **grandparent = nullptr;
            typename bs_tree<tkey, tvalue, tkey_comparer>::node *tree_root = (reinterpret_cast<splay_tree<tkey, tvalue, tkey_comparer> *>(this->_target_tree))->_root;
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** current_node = target_ptr;

            while ((*current_node) != tree_root) {
                parent = this->find_parent(path, current_node);

                if (tree_root == (*parent)) {
                    // zig
                    // rotate right/left: target_ptr becomes new _root if its grandparent is nullptr
                    if ((*parent)->left_subtree == (*current_node)) {
                        this->rotate_right(path, current_node);
                    } else {
                        this->rotate_left(path, current_node);
                    }
                    current_node = parent;
                    break;
                }
                else {
                    grandparent = this->find_grandparent(path, current_node);
                    if ((*parent)->left_subtree == (*current_node) && (*grandparent)->left_subtree == (*parent)) {
                        // zig-zig: rotate_right(parent) + rotate_right(target_ptr)
                        // stack: parent -> grandparent -> ...
                        path.pop();
                        parent = this->rotate_right(path, parent);
                        path.push(parent);
                        // stack: parent -> ...
                        current_node = this->rotate_right(path, current_node);
                        // stack: ...
                    }
                    else if ((*parent)->right_subtree == (*current_node) && (*grandparent)->right_subtree == (*parent))
                    {
                        // zig-zig: rotate_left(parent) + rotate_left(target_ptr)
                        // stack: parent -> grandparent -> ...
                        path.pop();
                        parent = this->rotate_left(path, parent);
                        path.push(parent);
                        // stack: parent -> ...
                        current_node = this->rotate_left(path, current_node);
                        // stack: ...
                    }
                    else if ((*parent)->right_subtree == (*current_node) && (*grandparent)->left_subtree == (*parent)) {
                        // zig-zag: rotate_left(target_ptr) + rotate_right(target_ptr)
                        current_node = this->rotate_left(path, current_node);
                        current_node = this->rotate_right(path, current_node);
                    } else {
                        // zig-zag: rotate_right(target_ptr) + rotate_left(target_ptr)
                        current_node = this->rotate_right(path, current_node);
                        current_node = this->rotate_left(path, current_node);
                    }
                }

                tree_root = /*reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics *>(const_cast<template_method_splay *>(this))->*/reinterpret_cast<splay_tree<tkey, tvalue, tkey_comparer> *>(this->_target_tree)->_root;
            }
        }

    public:
        explicit template_method_splay(
                splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
                {
            target_tree->_root = nullptr;
                }

            virtual ~template_method_splay() = default;
    };

#pragma region insertion splay tree
    class insertion_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
//        friend class template_method_splay;
        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
//            splay_tree<tkey, tvalue, tkey_comparer>::splay(path, target_ptr);
            reinterpret_cast<template_method_splay *>(this)->splay(path, target_ptr);
//            template_method_splay::splay(path, target_ptr);
        }

    public:
        explicit insertion_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(target_tree)
                {

                }
    };

#pragma endregion

#pragma region finding splay tree
    class finding_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method
    {
//        friend class template_method_splay;

        void after_find_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
//            reinterpret_cast<splay_tree<tkey, tvalue, tkey_comparer> *>(this->_target_tree)->splay(path, target_ptr);
//            template_method_splay::splay(path, target_ptr);
            reinterpret_cast<template_method_splay *>(this)->splay(path, target_ptr);
//            splay_tree<tkey, tvalue, tkey_comparer>::splay(path, target_ptr);
        }

    public:
        explicit finding_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method(target_tree)
        {

        }
    };

#pragma endregion

#pragma region removing splay tree
    class removing_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
//        friend class template_method_splay;

        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent_to_deleted_node = path.top();

            if (path.empty() == false) {
                path.pop();
                (reinterpret_cast<template_method_splay *>(const_cast<removing_splay_tree *>(this)))->splay(path, parent_to_deleted_node);
//                template_method_splay::splay(path, parent_to_deleted_node);
//                reinterpret_cast<splay_tree<tkey, tvalue, tkey_comparer> *>(this->_target_tree)->splay(path, parent_to_deleted_node);
//                splay_tree<tkey, tvalue, tkey_comparer>::splay(path, parent_to_deleted_node);
            }
        }

    public:
        explicit removing_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion
#pragma endregion

public:
    explicit splay_tree(
            logger *_logger = nullptr,
            memory *_allocator = nullptr)
            : bs_tree<tkey, tvalue, tkey_comparer>(
                    _logger,
                    _allocator,
                    new insertion_splay_tree(this),
                    new finding_splay_tree(this),
                    new removing_splay_tree(this))
    {

    }
};

#endif //SPLAY_TREE_H
