#include "splay_tree.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::template_splay_tree::splay(
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::node ** parent = nullptr, **grandparent = nullptr;

    while (*target_ptr != binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root)
    {
        parent = binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_parent(path, target_ptr);

        if (binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root == *parent) {
            // zig
            // rotate right/left: target_ptr becomes new _root if its grandparent is nullptr
            if ((*parent)->left_subtree == (*target_ptr)) {
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
            } else {
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
            }
        }
        else {
            grandparent = binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_grandparent(path, target_ptr);
            if ((*parent)->left_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                // zig-zig: rotate_right(parent) + rotate_right(target_ptr)
                // stack: parent -> grandparent -> ...
                path.pop();
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, parent);
                path.push(parent);
                // stack: parent -> ...
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                // stack: ...
            }
            else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->right_subtree == (*parent))
            {
                // zig-zig: rotate_left(parent) + rotate_left(target_ptr)
                // stack: parent -> grandparent -> ...
                path.pop();
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, parent);
                path.push(parent);
                // stack: parent -> ...
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                // stack: ...
            }
            else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                // zig-zag: rotate_left(target_ptr) + rotate_right(target_ptr)
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
            } else {
                // zig-zag: rotate_right(target_ptr) + rotate_left(target_ptr)
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
            }
        }
    }
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::insertion_splay_tree::after_insert_inner(
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    template_splay_tree::splay(path, target_ptr);
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::finding_splay_tree::after_find_inner(
        std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    template_splay_tree::splay(path, target_ptr);
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::removing_splay_tree::after_remove(
        const std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::node ** parent_to_deleted_node = path.top();
    path.pop();
    template_splay_tree::splay(path, parent_to_deleted_node);
}
