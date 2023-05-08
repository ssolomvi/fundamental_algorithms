#ifndef RB_TREE_H
#define RB_TREE_H

#include "../binary_tree/bs_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class rb_tree
        : public bs_tree<tkey, tvalue, tkey_comparer>
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
            if (path.empty()) {
                (*target_ptr)->change_color(rb_node::color::black);
                return;
            }
            /*
            if (this->_target_tree->_root == (*target_ptr)) {
                (*target_ptr)->change_color(rb_node::color::black);
                return;
            }
             */

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
                            if (reinterpret_cast<rb_node *>((*parent)->right_subtree) == (*current_node))
                            {
                                current_node = reinterpret_cast<rb_node **>(this->rotate_left(path, current_node));
                                parent = current_node;
                                current_node = reinterpret_cast<rb_node **>(&(*current_node)->left_subtree);
                            }
                            parent = this->rotate_right(path, parent);
                            grandparent = &((*parent)->right_subtree);
                        }
                        else if ((*grandparent)->right_subtree == (*parent)) {
                            if (reinterpret_cast<rb_node *>((*parent)->left_subtree) == (*current_node)) {
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

            if (path.empty()) {
                (*current_node)->change_color(rb_node::color::black);
            }
            /*
            if (this->_target_tree->_root == reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node *>(*current_node)) {
                (*current_node)->change_color(rb_node::color::black);
            }
             */
        }

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            auto ** current_node = reinterpret_cast<rb_node **>(target_ptr);
            (*current_node)->change_color(rb_node::red);

            do_balance(path, current_node);
        }

    public:
        explicit insertion_rb_tree(rb_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(target_tree)
        {

        }
    };

#pragma endregion

#pragma region removing rb tree
    class removing_rb_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        void swap_additional_data(
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *one_node,
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *another_node) override
        {
            auto *first  = reinterpret_cast<rb_node *>(one_node);
            auto *second = reinterpret_cast<rb_node *>(another_node);
            typename rb_node::color first_color = first->get_color();
            first->change_color(second->get_color());
            second->change_color(first_color);
        }

        tvalue remove(tkey const &key) override
        {
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = reinterpret_cast<rb_node **>(path_and_target.second);

            if (*target_ptr == nullptr)
            {
                throw typename bs_tree<tkey, tvalue, tkey_comparer>::bst_exception("finding_template_method::find:: no value with passed key in tree");
            }

            tvalue result = (*target_ptr)->value;

            // deleting element with 2 children (color does not matter)
            if ((*target_ptr)->left_subtree != nullptr &&
                (*target_ptr)->right_subtree != nullptr)
            {
                auto **element_to_swap_with = &(*target_ptr)->left_subtree;

                while ((*element_to_swap_with)->right_subtree != nullptr)
                {
                    path.push(element_to_swap_with);
                    element_to_swap_with = &(*element_to_swap_with)->right_subtree;
                }

                target_ptr = reinterpret_cast<rb_node **>(this->swap_nodes(element_to_swap_with, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr)));
//                target_ptr = reinterpret_cast<rb_node **>(element_to_swap_with);
            }

            typename rb_node::color target_ptr_color = (*target_ptr)->get_color();
            // deleting an element with no children
            if ((*target_ptr)->left_subtree == nullptr && (*target_ptr)->right_subtree == nullptr)
            {
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                if (target_ptr_color == rb_node::color::black) {
                    after_remove(path);
                    reinterpret_cast<rb_node *>(this->get_root_node())->change_color(rb_node::color::black);
                }
            }
            // deleting an element with 1 child (target_ptr is of black color)
            else if ((*target_ptr)->left_subtree != nullptr)
            {
                auto *target_left_subtree = (*target_ptr)->left_subtree;
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                *target_ptr = reinterpret_cast<rb_node *>(target_left_subtree);
                (*target_ptr)->change_color(rb_node::color::black);
            }
            else
            {
                auto *target_right_subtree = (*target_ptr)->right_subtree;
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                *target_ptr = reinterpret_cast<rb_node *>(target_right_subtree);
                (*target_ptr)->change_color(rb_node::color::black);
            }

            return result;
        }

        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // warning! color of root may change. Change color if needed
            if (path.empty()) {
                return;
            }
            rb_node ** parent = reinterpret_cast<rb_node **>(path.top());
            auto **brother_to_deleted = reinterpret_cast<rb_node **>(((*parent)->left_subtree == nullptr ?
                                                                      &((*parent)->right_subtree) : &((*parent)->left_subtree)));
            if ((*brother_to_deleted) == nullptr) {
                return;
            }

            rb_node **brother_right_subtree = reinterpret_cast<rb_node **>(&(*brother_to_deleted)->right_subtree);
            rb_node **brother_left_subtree = reinterpret_cast<rb_node **>(&(*brother_to_deleted)->left_subtree);

            if ((*brother_left_subtree) == nullptr && (*brother_right_subtree) == nullptr) {
                return;
            }

            rb_node **grandchild_left = nullptr, ** grandchild_right = nullptr;

            if ((*parent)->left_subtree == (*brother_to_deleted)) {
                if ((*parent)->get_color() == rb_node::color::red) {
                    (*brother_to_deleted)->change_color(rb_node::color::red);
                    (*parent)->change_color(rb_node::color::black);

                    if ((*brother_left_subtree)->get_color() == rb_node::color::red) {
                        (*brother_left_subtree)->change_color(rb_node::color::black);
                        this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                    }
                }
                // if parent color == black
                else {
                    grandchild_left = reinterpret_cast<rb_node **>(&((*brother_right_subtree)->left_subtree));
                    grandchild_right = reinterpret_cast<rb_node **>(&((*brother_right_subtree)->right_subtree));
                    if ((*brother_to_deleted)->get_color() == rb_node::color::red) {
                        if ((*grandchild_left)->get_color() == rb_node::color::black && (*grandchild_right)->get_color() == rb_node::color::black) {
                            (*brother_to_deleted)->change_color(rb_node::color::black);
                            (*brother_right_subtree)->change_color(rb_node::color::red);
                            this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                        }
                        else if ((*grandchild_left)->get_color() == rb_node::color::red) {
                            (*grandchild_left)->change_color(rb_node::color::black);
                            path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                            brother_right_subtree = this->rotate_left(path, brother_right_subtree);
                            this->rotate_right(path, brother_right_subtree);
                        }
                    }
                    // brother is black
                    else {
                        if ((*brother_right_subtree)->get_color() == rb_node::color::red &&
                            (*grandchild_left)->get_color() == rb_node::color::black &&
                            (*grandchild_right)->get_color() == rb_node::color::black) {
                            (*brother_right_subtree)->change_color(rb_node::color::black);
                            path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                            brother_right_subtree = this->rotate_left(path, brother_right_subtree);
                            this->rotate_right(path, brother_right_subtree);
                        }
                        else if ((*brother_left_subtree)->get_color() == rb_node::color::black &&
                                (*brother_right_subtree)->get_color() == rb_node::color::black) {
                            (*brother_to_deleted)->change_color(rb_node::color::red);
                            path.pop();
                            after_remove(path);
                        }
                    }
                }
            }
            else {
                if ((*parent)->get_color() == rb_node::color::red) {
                    (*brother_to_deleted)->change_color(rb_node::color::red);
                    (*parent)->change_color(rb_node::color::black);

                    if ((*brother_right_subtree)->get_color() == rb_node::color::red) {
                        (*brother_right_subtree)->change_color(rb_node::color::black);
                        this->rotate_left(path, brother_to_deleted);
                    }
                }
                // if parent color == black
                else {
                    grandchild_left = reinterpret_cast<rb_node **>(&((*brother_left_subtree)->left_subtree));
                    grandchild_right = reinterpret_cast<rb_node **>(&((*brother_left_subtree)->right_subtree));
                    if ((*brother_to_deleted)->get_color() == rb_node::color::red) {
                        if ((*grandchild_left)->get_color() == rb_node::color::black && (*grandchild_right)->get_color() == rb_node::color::black) {
                            (*brother_to_deleted)->change_color(rb_node::color::black);
                            (*brother_left_subtree)->change_color(rb_node::color::red);
                            this->rotate_left(path, brother_to_deleted);
                        }
                        else if ((*grandchild_right)->get_color() == rb_node::color::red) {
                            (*grandchild_right)->change_color(rb_node::color::black);
                            path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                            brother_left_subtree = this->rotate_right(path, brother_left_subtree);
                            this->rotate_left(path, brother_left_subtree);
                        }
                    }
                    // brother is black
                    else {
                        if ((*brother_left_subtree)->get_color() == rb_node::color::red &&
                            (*grandchild_left)->get_color() == rb_node::color::black &&
                            (*grandchild_right)->get_color() == rb_node::color::black) {
                            (*brother_right_subtree)->change_color(rb_node::color::black);
                            path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                            brother_left_subtree = this->rotate_right(path, brother_left_subtree);
                            this->rotate_right(path, brother_left_subtree);
                        }
                        else if ((*brother_left_subtree)->get_color() == rb_node::color::black &&
                                 (*brother_right_subtree)->get_color() == rb_node::color::black) {
                            (*brother_to_deleted)->change_color(rb_node::color::red);
                            path.pop();
                            after_remove(path);
                        }
                    }
                }
            }
            // TODO: check if current root color is black ?
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
