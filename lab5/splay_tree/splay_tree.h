#ifndef LAB5_SPLAY_TREE_H
#define LAB5_SPLAY_TREE_H

#include "../binary_tree/bst_tree.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
class splay_tree final
        : public bst_tree<tkey, tvalue, tkey_comparer>
{

protected:
    class template_splay_tree final
            : public bst_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
        void splay(std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr);
    };

    class insertion_splay_tree final :
            public bst_tree<tkey, tvalue, tkey_comparer>::insertion_template_method,
            public template_splay_tree
    {

        void after_insert_inner(std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override;
    };

    class finding_splay_tree final :
            public bst_tree<tkey, tvalue, tkey_comparer>::finding_template_method,
            public template_splay_tree
    {
        void after_find_inner(std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bst_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override;
    };

    class removing_splay_tree final :
            public bst_tree<tkey, tvalue, tkey_comparer>::removing_template_method,
            public template_splay_tree
    {
        virtual void after_remove(std::stack<typename bst_tree<tkey, tvalue, tkey_comparer>::node **> const &path);
    };

};

#endif //LAB5_SPLAY_TREE_H
