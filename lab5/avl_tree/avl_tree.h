#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "../binary_tree/bs_tree.h"

// TODO: add get node size

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class avl_tree final
        : public bs_tree<tkey, tvalue, tkey_comparer>
//          public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
{
    struct avl_node
            : bs_tree<tkey, tvalue, tkey_comparer>::node
    {
        unsigned height;

        unsigned get_height()
        {
            return (this == nullptr ? 0 : height);
        }

        int get_balance()
        {
            return (this == nullptr ? 0 : this->left_subtree->get_height() - this->right_subtree->get_height());
        }

        unsigned get_max_height_of_two_nodes(avl_node * another)
        {
            unsigned this_h = get_height(this), another_h = get_height(another);
            return (this_h > another_h ? this_h : another_h);
        }
    };

protected:
#pragma region template methods avl tree
    class template_methods_avl :
        public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
//        friend class insertion_avl_tree;
//        friend class removing_avl_tree;
    public:
        // TODO: check if this works
        void rotate_fix_additional_data(typename bs_tree<tkey, tvalue, tkey_comparer>::node * curr_node) const override
        {
            auto * tmp = reinterpret_cast<avl_node *>(curr_node);
            tmp->height = reinterpret_cast<avl_node *>(tmp->left_subtree)->get_max_height_of_two_nodes(reinterpret_cast<avl_node *>(tmp->right_subtree) + 1);
        }

        void do_balance(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, avl_node **target_ptr)
        {
            avl_node ** current_node = target_ptr;
            int balance, left_subtree_balance, right_subtree_balance;

            do {
                balance = (*current_node)->get_balance();

                left_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->left_subtree)->get_balance();
                right_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->right_subtree)->get_balance();

                if (balance > 1) {
                    if (left_subtree_balance >= 0) {
                        this->rotate_right((*current_node)->left_subtree);
                    } else {
                        this->rotate_left((*current_node)->left_subtree->right_subtree);
                        this->rotate_right((*current_node)->left_subtree);
                    }
                } else if (balance < 1) {
                    if (right_subtree_balance <= 0) {
                        this->rotate_left((*current_node)->right_subtree);
                    } else {
                        this->rotate_right((*current_node)->right_subtree->left_subtree);
                        this->rotate_left((*current_node)->right_subtree);
                    }
                }

                if (path.empty()) {
                    break;
                }
                current_node = reinterpret_cast<avl_node **>(path.top());
                path.pop();
            } while (true);
        }

    public:
        explicit template_methods_avl(
                avl_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
        {

        }
    };

#pragma region insertion avl tree
    class insertion_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
        size_t get_node_size() const override
        {
            return sizeof(avl_node);
        }

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            // 1) insert additional data to this node
            reinterpret_cast<avl_node *>((*target_ptr))->height = 1;

//            2) go to grandparent of this target_ptr (if it exists)
            auto ** grandparent = reinterpret_cast<avl_node **>(this->find_grandparent(path, target_ptr));
            if (grandparent == nullptr) {
                return;
            }
            path.pop();
            path.pop();

//            3) do_balance
            reinterpret_cast<template_methods_avl *>(this)->do_balance(path, grandparent);
//            template_methods_avl::do_balance();
        }

    public:
        explicit insertion_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion

    /*
    // TODO: нам на фиг не нужен этот класс, мы ни фига не делаем после нахождения конкретной ноды
    class finding_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method
    {
        void after_find_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            // nothing to do here in avl_tree context
        }
    public:
        explicit finding_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };
     */
#pragma region removing avl tree
    class removing_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) override
        {
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // 1) прийти к деду
            path.pop();
            if (path.empty()) {
                return;
            }
            auto ** grandparent = reinterpret_cast<avl_node **>(path.top());
            path.pop();

            // 2) do_balance
            reinterpret_cast<template_methods_avl *>(this)->do_balance(path, grandparent);
//            template_methods_avl::do_balance(path, grandparent);
        }

        void swap_additional_data(
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *one_node,
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *another_node) override
        {
            auto* one = reinterpret_cast<avl_node*>(one_node), *second = reinterpret_cast<avl_node *>(another_node);
            unsigned height_tmp = one->get_height();
            one->height = second->height;
            second->height = height_tmp;
        }

    public:
        explicit removing_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion
#pragma endregion

public:
    explicit avl_tree(
            logger *_logger = nullptr,
            memory *_allocator = nullptr)
            : bs_tree<tkey, tvalue, tkey_comparer>(
                _logger,
                _allocator,
                new insertion_avl_tree(this),
                new typename bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method(this),
                new removing_avl_tree(this))
    {

    }
};

#endif //AVL_TREE_H
