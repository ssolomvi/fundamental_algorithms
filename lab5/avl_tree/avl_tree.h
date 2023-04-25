#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "../binary_tree/bs_tree.h"

// ask как переопределить узел чтоб right и left subtree тоже были узлами типа наследника node

template<typename tkey, typename tvalue, typename tkey_comparer>
class avl_tree
        : public bs_tree<tkey, tvalue, tkey_comparer>,
          public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
{
    struct avl_node
    {
        avl_node * right_subtree, *left_subtree;
//        tkey key;
//        tvalue value;
        unsigned height;
    };

    static unsigned get_height(avl_node *target_ptr)
    {
        return (target_ptr == nullptr ? 0 : target_ptr->height);
    }

    static int get_balance(avl_node *target_ptr)
    {
        return (target_ptr == nullptr ? 0 : (get_height(target_ptr->left_subtree) - get_height(target_ptr->right_subtree)));
    }

    static unsigned get_max_height_of_two_nodes(avl_node * first, avl_node * second) {
        unsigned first_h = get_height(first), second_h = get_height(second);
        return (first_h > second_h ? first_h : second_h);
    }

    void rotate_fix_additional_data(avl_node * node)
    {
        node->height = (get_max_height_of_two_nodes(node->left_subtree, node->right_subtree) + 1);
    }

    void do_balance(std::stack<avl_node **> &path, avl_node **target_ptr)
    {
        avl_node ** current_node = target_ptr;
        int balance, left_subtree_balance, right_subtree_balance;

        do {
            balance = get_balance(*current_node);

            left_subtree_balance = get_balance((*current_node)->left_subtree);
            right_subtree_balance = get_balance((*current_node)->right_subtree);

            if (balance > 1) {
                if (left_subtree_balance >= 0) {
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right((*current_node)->left_subtree);
                } else {
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left((*current_node)->left_subtree->right_subtree);
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right((*current_node)->left_subtree);
                }
            } else if (balance < 1) {
                if (right_subtree_balance <= 0) {
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left((*current_node)->right_subtree);
                } else {
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_right((*current_node)->right_subtree->left_subtree);
                    bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics::rotate_left((*current_node)->right_subtree);
                }
            }

            current_node = path.top();
            path.pop();
        } while (!path.empty());
    }


protected:
    class insertion_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            // TODO: 1) insert additional data to this node, 2) go to grandparent of this target_ptr (if it exists), 3) do_balance
        }

    public:
        explicit insertion_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
                : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

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

    class removing_avl_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) override
        {
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // todo: прийти к деду, do_balance
        }

    public:
        explicit removing_avl_tree(avl_tree<tkey, tvalue, tkey_comparer> * target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::removing_template_method(target_tree)
        {

        }
    };

};

#endif //AVL_TREE_H
