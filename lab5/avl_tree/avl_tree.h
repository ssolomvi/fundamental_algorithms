#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "../binary_tree/bs_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class avl_tree final
        : public bs_tree<tkey, tvalue, tkey_comparer>
{
protected:
    struct avl_node
            : bs_tree<tkey, tvalue, tkey_comparer>::node
    {
    protected:
        unsigned height;

    public:
        unsigned int get_height()
        {
            return (this == nullptr ? 0 : height);
        }

        void set_height(unsigned new_h)
        {
            this->height = new_h;
        }

        int get_balance()
        {
            return (this == nullptr ? 0 : reinterpret_cast<avl_node *>(this->left_subtree)->get_height() -
                                          reinterpret_cast<avl_node *>(this->right_subtree)->get_height());
        }

    public:
        unsigned get_max_height_of_two_nodes(avl_node * another)
        {
            unsigned this_h = this->get_height(), another_h = another->get_height();
            return (this_h > another_h ? this_h : another_h);
        }

        void update_height()
        {
            height = reinterpret_cast<avl_node *>(this->left_subtree)->get_max_height_of_two_nodes(reinterpret_cast<avl_node *>(this->right_subtree)) + 1;
        }
    };

protected:
#pragma region template methods avl tree
    class template_methods_avl :
        public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
        /*
         // does not work
        void rotate_fix_additional_data(typename bs_tree<tkey, tvalue, tkey_comparer>::node * target_ptr) const override
        {
            auto * tmp = reinterpret_cast<avl_node *>(target_ptr);
            tmp->set_height(reinterpret_cast<avl_node *>(tmp->left_subtree)->get_max_height_of_two_nodes(reinterpret_cast<avl_node *>(tmp->right_subtree)) + 1);
        }
         */

    public:
        void do_balance(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, avl_node **target_ptr)
        {
            avl_node ** current_node = target_ptr;
            int balance, left_subtree_balance, right_subtree_balance;

            do {
                balance = (*current_node)->get_balance();

                left_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->left_subtree)->get_balance();
                right_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->right_subtree)->get_balance();

                if (balance > 1) {
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node));
                    // to_do: add path
                    // returned by rotations values are current top node
                    if (left_subtree_balance >= 0) {
                        this->rotate_right(path, &((*current_node)->left_subtree));
                    } else {
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(&((*current_node)->left_subtree)));
                        this->rotate_left(path, &((*current_node)->left_subtree->right_subtree));
                        reinterpret_cast<avl_node *>((*current_node)->left_subtree->left_subtree)->update_height();
                        reinterpret_cast<avl_node *>((*current_node)->left_subtree)->update_height();

                        this->rotate_right(path, &((*current_node)->left_subtree));
                    }
                    reinterpret_cast<avl_node *>((*current_node)->right_subtree)->update_height();
                    reinterpret_cast<avl_node *>((*current_node))->update_height();

                } else if (balance < -1) {
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node));
                    if (right_subtree_balance <= 0) {
                        this->rotate_left(path, &((*current_node)->right_subtree));
                    } else {
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(&((*current_node)->right_subtree)));
                        this->rotate_right(path, &((*current_node)->right_subtree->left_subtree));
                        reinterpret_cast<avl_node *>((*current_node)->right_subtree->right_subtree)->update_height();
                        reinterpret_cast<avl_node *>((*current_node)->right_subtree)->update_height();

                        this->rotate_left(path, &((*current_node)->right_subtree));
                    }
                    reinterpret_cast<avl_node *>((*current_node)->left_subtree)->update_height();
                    reinterpret_cast<avl_node *>((*current_node))->update_height();
                }

                if (path.empty()) {
                    break;
                }
                current_node = reinterpret_cast<avl_node **>(path.top());
                path.pop();
                (*current_node)->update_height();
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

        void initialize_memory_with_node(
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *target_ptr) const override
        {
            new(reinterpret_cast<avl_node *>(target_ptr)) avl_node;
        }

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            // 1) insert additional data to this node
            reinterpret_cast<avl_node *>((*target_ptr))->set_height(1);

            avl_node ** parent = reinterpret_cast<avl_node **>(this->find_parent(path, target_ptr));
            if (parent == nullptr) {
                return;
            }
            (*parent)->update_height();

//            2) go to grandparent of this target_ptr (if it exists)
            auto ** grandparent = reinterpret_cast<avl_node **>(this->find_grandparent(path, target_ptr));
            if (grandparent == nullptr) {
                return;
            }
            (*grandparent)->update_height();
            path.pop();
            path.pop();

//            3) do_balance
            reinterpret_cast<template_methods_avl *>(this)->do_balance(path, grandparent);
//            template_methods_avl::do_balance();
        }

    public:
        explicit insertion_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(target_tree)
        {

        }
    };

#pragma endregion


    // TODO: нам на фиг не нужен этот класс, мы ни фига не делаем после нахождения конкретной ноды
    class finding_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method
    {
        /*
        void after_find_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            // nothing to do here in avl_tree context
        }
         */
    public:
        explicit finding_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method(target_tree)
        {

        }
    };

#pragma region removing avl tree
    class removing_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // 1) прийти к родителю
            if (path.empty()) {
                return;
            }
            avl_node ** parent = reinterpret_cast<avl_node **>(path.top());
            (*parent)->update_height();
            path.pop();

//            auto ** grandparent = reinterpret_cast<avl_node **>(path.top());
//            path.pop();

            // 2) do_balance
            reinterpret_cast<template_methods_avl *>(const_cast<removing_avl_tree *>(this))->do_balance(path, parent);
//            template_methods_avl::do_balance(path, grandparent);
        }

        void swap_additional_data(
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *one_node,
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *another_node) override
        {
            avl_node * one = reinterpret_cast<avl_node *>(one_node), *second = reinterpret_cast<avl_node *>(another_node);
            unsigned height_tmp = one->get_height();
            one->set_height(second->get_height());
            second->set_height(height_tmp);
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
                new finding_avl_tree(this),
                new removing_avl_tree(this))
    {
    }
};

#endif //AVL_TREE_H
