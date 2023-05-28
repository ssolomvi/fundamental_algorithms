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

                tree_root = /*reinterpret_cast<typename bs_tree<key, tvalue, tkey_comparer>::template_method_basics *>(const_cast<template_method_splay *>(this))->*/reinterpret_cast<splay_tree<tkey, tvalue, tkey_comparer> *>(this->_target_tree)->_root;
            }
        }

    public:
        explicit template_method_splay(
                splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
                {
            //target_tree->_root = nullptr;
                }

            virtual ~template_method_splay() = default;
    };

#pragma region insertion splay tree
    class insertion_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            this->trace_with_guard("splay_tree::insertion_splay_tree::after_insert_inner::splay method started");
            reinterpret_cast<template_method_splay *>(this)->splay(path, target_ptr);
            this->trace_with_guard("splay_tree::insertion_splay_tree::after_insert_inner::splay method finished");
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
        void after_find_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            this->trace_with_guard("splay_tree::finding_splay_tree::after_find_inner::splay method started");
            reinterpret_cast<template_method_splay *>(this)->splay(path, target_ptr);
            this->trace_with_guard("splay_tree::finding_splay_tree::after_find_inner::splay method started");
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
        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            this->trace_with_guard("splay_tree::removing_splay_tree::after_remove::splay method started");
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent_to_deleted_node = path.top();

            if (path.empty() == false) {
                path.pop();
                (reinterpret_cast<template_method_splay *>(const_cast<removing_splay_tree *>(this)))->splay(path, parent_to_deleted_node);
            }
            this->trace_with_guard("splay_tree::removing_splay_tree::after_remove::splay method started");
        }

    public:
        explicit removing_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion
#pragma endregion

#pragma region rule 5
public:
    explicit splay_tree(logger *_logger = nullptr, memory *_allocator = nullptr)
            : bs_tree<tkey, tvalue, tkey_comparer>(
                    _logger,
                    _allocator,
                    new insertion_splay_tree(this),
                    new finding_splay_tree(this),
                    new removing_splay_tree(this))
    {

    }

private:
    // constructor
    splay_tree(
            logger *logger,
            memory *allocator,
            insertion_splay_tree *insertion,
            finding_splay_tree *finding,
            removing_splay_tree *removing)
    {
        this->_logger(logger);
        this->_allocator(allocator);
        this->_insertion(insertion);
        this->_finding(finding);
        this->_removing(removing);
        this->_root(nullptr);

        this->trace_with_guard("splay_tree constructor was called");
    }

public:
    // copy constructor
    splay_tree(splay_tree<tkey, tvalue, tkey_comparer> const &obj)
            : splay_tree(obj._logger, obj._allocator)
    {
        this->trace_with_guard("splay_tree copy constructor was called");
        this->_root = this->copy(obj._root);
    }

    // move constructor
    splay_tree(splay_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    : splay_tree(obj._insertion,
                 obj._finding,
                 obj._removing,
                 obj._allocator,
                 obj._logger)
    {
        this->trace_with_guard("splay_tree move constructor was called");

        this->_root = obj._root;
        obj._root = nullptr;

        this->_insertion->_target_tree = this;
        obj._insertion = nullptr;

        this->_finding->_target_tree = this;
        obj._finding = nullptr;

        this->_removing->_target_tree = this;
        obj._removing = nullptr;

        obj._allocator = nullptr;

        obj._logger = nullptr;
    }

    // copy assignment (оператор присваивания)
    splay_tree &operator=(splay_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        this->trace_with_guard("splay_tree copy assignment constructor was called");

        if (this == &obj)
        {
            return *this;
        }

        this->clearup(this->_root);

        this->_allocator = obj._allocator;
        this->_logger = obj._logger;
        this->_root = this->copy(obj._root);

        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    splay_tree &operator=(splay_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        this->trace_with_guard("splay_tree move assignment constructor was called");
        if (this == &obj)
        {
            return *this;
        }

        clearup(this->_root);
        this->_root = obj._root;
        obj._root = nullptr;

        delete obj._insertion;
        obj._insertion = nullptr;

        delete obj._finding;
        obj._finding = nullptr;

        delete obj._removing;
        obj._removing = nullptr;

        this->_allocator = obj._allocator;
        obj._allocator = nullptr;

        this->_logger = obj._logger;
        obj._logger = nullptr;

        return *this;
    }

    // destructor
    ~splay_tree()
    {
        this->trace_with_guard("splay_tree destructor was called");
/*
        delete this->_insertion;
        delete this->_finding;
        delete this->_removing;

        this->clearup(this->_root);
  */
    }
#pragma endregion
};

#endif //SPLAY_TREE_H
