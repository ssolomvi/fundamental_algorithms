#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include "../binary_tree/bs_tree.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
class splay_tree final
        : public bs_tree<tkey, tvalue, tkey_comparer>
{
protected:


protected:
    class template_method_splay
            : public binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
    public:
        void splay(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
        {
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent, **grandparent;

            while (*target_ptr != bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root)
            {
                parent = bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_parent(path, target_ptr);

                if (bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root == *parent) {
                    // zig
                    // rotate right/left: target_ptr becomes new _root if its grandparent is nullptr
                    if ((*parent)->left_subtree == (*target_ptr)) {
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                    } else {
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                    }
                }
                else {
                    grandparent = bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_grandparent(path, target_ptr);
                    if ((*parent)->left_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                        // zig-zig: rotate_right(parent) + rotate_right(target_ptr)
                        // stack: parent -> grandparent -> ...
                        path.pop();
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, parent);
                        path.push(parent);
                        // stack: parent -> ...
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                        // stack: ...
                    }
                    else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->right_subtree == (*parent))
                    {
                        // zig-zig: rotate_left(parent) + rotate_left(target_ptr)
                        // stack: parent -> grandparent -> ...
                        path.pop();
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, parent);
                        path.push(parent);
                        // stack: parent -> ...
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                        // stack: ...
                    }
                    else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                        // zig-zag: rotate_left(target_ptr) + rotate_right(target_ptr)
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                    } else {
                        // zig-zag: rotate_right(target_ptr) + rotate_left(target_ptr)
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                        bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                    }
                }
            }
        }

    public:
        explicit template_method_splay(
                bs_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
                {

                }

            virtual ~template_method_splay() = default;
    };


    class insertion_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
        friend class template_method_splay;

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            template_method_splay::splay(path, target_ptr);
        }

    public:
        explicit insertion_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(target_tree)
                {

                }
    };

    class finding_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method
    {
//        friend class splay_tree<tkey, tvalue, tkey_comparer>;
        friend class template_method_splay;

        void after_find_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            template_method_splay::splay(path, target_ptr);
        }

    public:
        explicit finding_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method(target_tree)
        {

        }
    };

    class removing_splay_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
//        friend class splay_tree<tkey, tvalue, tkey_comparer>;
        friend class template_method_splay;

        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            typename bs_tree<tkey, tvalue, tkey_comparer>::node ** parent_to_deleted_node = path.top();
            if (path.empty() == false) {
                path.pop();
                template_method_splay::splay(path, parent_to_deleted_node);
            }
        }

    public:
        explicit removing_splay_tree(splay_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

public:
    explicit splay_tree(
            logger *logger = nullptr,
            memory *allocator = nullptr)
            : bs_tree<tkey, tvalue, tkey_comparer>(
                    logger,
                    allocator,
                    new insertion_splay_tree(this),
                    new finding_splay_tree(this),
                    new removing_splay_tree(this))
    {

    }
};

#endif //SPLAY_TREE_H
