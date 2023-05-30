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
    public:
        void do_balance(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, avl_node **target_ptr)
        {
            avl_node ** current_node = target_ptr;
            int balance, left_subtree_balance, right_subtree_balance;

            do {
                balance = (*current_node)->get_balance();

                left_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->left_subtree)->get_balance();
                right_subtree_balance = reinterpret_cast<avl_node *>((*current_node)->right_subtree)->get_balance();

                if (balance > 1) { // == 2
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
            this->trace_with_guard("avl_tree::insertion_avl_tree::after_insert_inner method started");
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
            this->trace_with_guard("avl_tree::template_methods_avl::do_balance method started");
            reinterpret_cast<template_methods_avl *>(this)->do_balance(path, grandparent);
            this->trace_with_guard("avl_tree::template_methods_avl::do_balance method finished")
                ->trace_with_guard("avl_tree::insertion_avl_tree::after_insert_inner method finished");
        }

    public:
        explicit insertion_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(target_tree)
        {

        }
    };

#pragma endregion

#pragma region removing avl tree
    class removing_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            this->trace_with_guard("avl_tree::removing_avl_tree::after_remove method started");
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // 1) прийти к родителю
            if (path.empty()) {
                return;
            }
            avl_node ** parent = reinterpret_cast<avl_node **>(path.top());
            (*parent)->update_height();
            path.pop();

            // 2) do_balance
            this->trace_with_guard("avl_tree::template_methods_avl::do_balance method started");
            reinterpret_cast<template_methods_avl *>(const_cast<removing_avl_tree *>(this))->do_balance(path, parent);
            this->trace_with_guard("avl_tree::template_methods_avl::do_balance method finished")
                ->trace_with_guard("avl_tree::removing_avl_tree::after_remove method started");
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

#pragma region rule 5
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

private:
    // constructor
    avl_tree(
            logger *logger,
            memory *allocator,
            insertion_avl_tree *insertion,
            typename bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method *finding,
            removing_avl_tree *removing)
    {
        this->_logger(logger);
        this->_allocator(allocator);
        this->_insertion(insertion);
        this->_finding(finding);
        this->_removing(removing);
        this->_root(nullptr);

        this->trace_with_guard("avl_tree constructor was called");
    }

public:
    // copy constructor
    avl_tree(avl_tree<tkey, tvalue, tkey_comparer> const &obj)
            : avl_tree(obj._logger, obj._allocator)
    {
        this->trace_with_guard("avl_tree copy constructor was called");
        this->_root = this->copy(obj._root);
    }

    // move constructor
    avl_tree(avl_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    : avl_tree(obj._insertion,
               obj._finding,
               obj._removing,
               obj._allocator,
               obj._logger)
    {
        this->trace_with_guard("avl_tree move constructor was called");

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
    avl_tree &operator=(avl_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        this->trace_with_guard("avl_tree copy assignment constructor was called");

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
    avl_tree &operator=(avl_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        this->trace_with_guard("avl_tree move assignment constructor was called");

        if (this == &obj)
        {
            return *this;
        }

        this->clearup(this->_root);
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
    ~avl_tree()
    {
        this->trace_with_guard("avl_tree destructor was called");
    }

private:
//    void clearup(typename bs_tree<tkey, tvalue, tkey_comparer>::node *element) override
//    {
//        if (element == nullptr)
//        {
//            return;
//        }
//
//        clearup(element->left_subtree);
//        clearup(element->right_subtree);
//
//        reinterpret_cast<avl_node *>(element)->~avl_node();
//        this->deallocate_with_guard(element);
//    }

    typename bs_tree<tkey, tvalue, tkey_comparer>::node *copy(typename bs_tree<tkey, tvalue, tkey_comparer>::node *from) override
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        avl_node *result = reinterpret_cast<avl_node *>(this->allocate_with_guard(sizeof(avl_node)));
        new (result) avl_node(*reinterpret_cast<avl_node *>(from));
        result->set_height(reinterpret_cast<avl_node *>(from)->get_height());

        result->left_subtree = copy(from->left_subtree);
        result->right_subtree = copy(from->right_subtree);

        return result;
    }

#pragma endregion

};

#endif //AVL_TREE_H
