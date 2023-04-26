#ifndef RB_TREE_H
#define RB_TREE_H

#include "../binary_tree/bs_tree.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
class rb_tree
        : public bs_tree<tkey, tvalue, tkey_comparer>,
          public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
{
    struct rb_node
        : public bs_tree<tkey, tvalue, tkey_comparer>::node
    {
        enum color {
            red,
            black
        };

    private:
        color _color;

    public:
        color get_color()
        {
            return _color;
        }

        void change_color(color color_to_set)
        {
            _color = color_to_set;
        }
    };

protected:
#pragma region template methods rb tree
    class insertion_rb_tree;
    class removing_rb_tree;

    class template_methods_rb :
            public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
        friend class insertion_rb_tree;
        friend class removing_rb_tree;

    public:
        explicit template_methods_rb(rb_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
        {

        }
    };

#pragma region insertion to rb tree
    class insertion_rb_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {

        rb_node** find_uncle(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, rb_node** current_node)
        {
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent = path.top();
            path.pop();
            if (path.empty()) {
                path.push(parent);
                return nullptr;
            }
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** grandparent = path.top();
            rb_node ** uncle = ((*grandparent)->left_subtree == (*parent) ?
                    reinterpret_cast<rb_node **>(&((*grandparent)->right_subtree))
                    :
                    reinterpret_cast<rb_node **>(&((*grandparent)->left_subtree))
                    );

            path.push(parent);
            return uncle;
        }

        void do_balance(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, rb_node **target_ptr)
        {
            if (this->_target_tree->_root == (*target_ptr)) {
                (*target_ptr)->change_color(rb_node::color::black);
                return;
            }

            rb_node ** current_node = target_ptr;

            auto ** parent = reinterpret_cast<rb_node **>(this->find_parent(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
            auto ** grandparent = reinterpret_cast<rb_node **>(this->find_grandparent(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
            rb_node ** uncle = find_uncle(path, current_node);

            if (parent == nullptr) {
                return;
            }

            if ((*parent)->get_color() == rb_node::color::red) {
                while ((*parent)->get_color() == rb_node::color::red) {
                    if (grandparent == nullptr || uncle == nullptr) {
                        return;
                    }

                    if ((*uncle)->get_color() == rb_node::color::red) {
                        (*grandparent)->change_color(rb_node::color::red);
                        (*parent)->change_color(rb_node::color::black);
                        (*uncle)->change_color(rb_node::color::black);
                        current_node = grandparent;
                    }
                    else {
                        if ((*grandparent)->left_subtree == (*parent)) {
                            if ((*parent)->right_subtree == current_node) {
                                current_node = reinterpret_cast<rb_node **>(this->rotate_left(path, current_node));
                                parent = current_node;
                                current_node = reinterpret_cast<rb_node **>(&(*current_node)->left_subtree);
                            }
                            parent = this->rotate_right(path, parent);
                            grandparent = &((*parent)->right_subtree);
                        }
                        else if ((*grandparent)->right_subtree == (*parent)) {
                            if ((*parent)->left_subtree == current_node) {
                                current_node = this->rotate_right(path, current_node);
                                parent = current_node;
                                current_node = reinterpret_cast<rb_node **>(&(*current_node)->right_subtree);
                            }
                            parent = this->rotate_left(path, parent);
                            grandparent = &((*parent)->left_subtree);
                        }
                        (*parent)->change_color(rb_node::color::black);
                        (*grandparent)->change_color(rb_node::color::red);
                        current_node = parent;
                    }

                    parent = reinterpret_cast<rb_node **>(this->find_parent(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
                    grandparent = reinterpret_cast<rb_node **>(this->find_grandparent(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
                    uncle = find_uncle(path, current_node);
                }
            }

            if (this->_target_tree->_root == reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node *>(*current_node)) {
                (*current_node)->change_color(rb_node::color::black);
            }
        }

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            auto ** current_node = reinterpret_cast<rb_node **>(target_ptr);
            (*current_node)->change_color(rb_node::red);

            do_balance(path, current_node);
        }

    public:
        explicit insertion_rb_tree(rb_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion

#pragma region removing rb tree
    class removing_rb_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        tvalue &&remove(tkey const &key) override
        {
            // todo: delete a node following rb remove rules
            // todo: call after_remove in case we delete a black node without children
        }

        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) override
        {
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
        }

    public:
        explicit removing_rb_tree(rb_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

#pragma endregion

#pragma endregion

public:
    explicit rb_tree(
            logger *_logger = nullptr,
            memory *_allocator = nullptr)
    : bs_tree<tkey, tvalue, tkey_comparer>(
            _logger,
            _allocator,
    new insertion_rb_tree(this),
    new typename bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method(this),
    new removing_rb_tree(this))
    {

    }


};


#endif //RB_TREE_H
