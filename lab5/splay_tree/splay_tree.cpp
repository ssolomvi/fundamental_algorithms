#include "splay_tree.h"

/*
template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::splay(
        std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    typename bst_tree<tkey, tvalue, tkey_comparer>::node ** parent = nullptr, **grandparent = nullptr;

    while (*target_ptr != bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root)
    {
        parent = bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_parent(path, target_ptr);

        if (bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::_target_tree->_root == *parent) {
            // zig
            // rotate right/left: target_ptr becomes new _root if its grandparent is nullptr
            if ((*parent)->left_subtree == (*target_ptr)) {
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
            } else {
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
            }
        }
        else {
            grandparent = bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_grandparent(path, target_ptr);
            if ((*parent)->left_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                // zig-zig: rotate_right(parent) + rotate_right(target_ptr)
                // stack: parent -> grandparent -> ...
                path.pop();
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, parent);
                path.push(parent);
                // stack: parent -> ...
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                // stack: ...
            }
            else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->right_subtree == (*parent))
            {
                // zig-zig: rotate_left(parent) + rotate_left(target_ptr)
                // stack: parent -> grandparent -> ...
                path.pop();
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, parent);
                path.push(parent);
                // stack: parent -> ...
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                // stack: ...
            }
            else if ((*parent)->right_subtree == (*target_ptr) && (*grandparent)->left_subtree == (*parent)) {
                // zig-zag: rotate_left(target_ptr) + rotate_right(target_ptr)
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
            } else {
                // zig-zag: rotate_right(target_ptr) + rotate_left(target_ptr)
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right(path, target_ptr);
                bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left(path, target_ptr);
            }
        }
    }
}
*/
/*
template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::insertion_splay_tree::after_insert_inner(
        std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    splay(path, target_ptr);
}
*/
/*
template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::finding_splay_tree::after_find_inner(
        std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path,
        typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
{
    splay(path, target_ptr);
}
*/
/*
template<typename tkey, typename tvalue, typename tkey_comparer>
void splay_tree<tkey, tvalue, tkey_comparer>::removing_splay_tree::after_remove(
        const std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path)
{
    typename bst_tree<tkey, tvalue, tkey_comparer>::node ** parent_to_deleted_node = path.top();
    path.pop();
    splay(path, parent_to_deleted_node);
}
*/