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
        enum _color {
            red,
            black
        };

        rb_node * left_subtree, *right_subtree;
        _color color;
    };


    

};


#endif //RB_TREE_H
