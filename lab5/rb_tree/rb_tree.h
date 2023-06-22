#ifndef RB_TREE_H
#define RB_TREE_H
#define RED true
#define BLACK false

#include "../binary_tree/bs_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class rb_tree
        : public bs_tree<tkey, tvalue, tkey_comparer>
{
protected:
    struct rb_node
        : public bs_tree<tkey, tvalue, tkey_comparer>::node
    {
        /*
    enum color {
        red = 0,
        black = 1
    };


    protected:
        color _color;

    public:
        color get_color()
        {
            return (this == nullptr ? black : _color);
        }

        void change_color(color color_to_set)
        {
            _color = color_to_set;
        }
         */
    private:
        // red == 1, black = 0
        bool _color;

    public:
        bool is_red()
        {
            return _color;
        }

        bool is_black()
        {
            return (!_color);
        }

        bool get_color()
        {
            return (this == nullptr ? BLACK : _color);
        }

        void change_color(bool new_color)
        {
            _color = new_color;
        }
    };

protected:
#pragma region template methods rb tree
    /*
    class template_methods_rb :
            public bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics
    {
    public:
        explicit template_methods_rb(rb_tree<tkey, tvalue, tkey_comparer> *target_tree)
            : bs_tree<tkey, tvalue, tkey_comparer>::template_method_basics(target_tree)
        {

        }
    };
     */

#pragma region insertion to rb tree
    class insertion_rb_tree final :
            public bs_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
    private:
        size_t get_node_size() const override
        {
            return sizeof(rb_node);
        }

        void initialize_memory_with_node(
                typename bs_tree<tkey, tvalue, tkey_comparer>::node *target_ptr) const override
        {
            new(reinterpret_cast<rb_node *>(target_ptr)) rb_node;
        }

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
            return ((*uncle) == nullptr ? nullptr : uncle);
        }

        void insertion_do_balance(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr)
        {
            rb_node ** current_node = reinterpret_cast<rb_node **>(target_ptr);

            // if current node is root make it black
            if (path.empty()) {
                (*current_node)->change_color(BLACK);
                return;
            }

            rb_node ** parent = reinterpret_cast<rb_node **>(this->find_parent(path, target_ptr));
            rb_node ** grandparent = reinterpret_cast<rb_node **>(this->find_grandparent(path, target_ptr));

            if (parent == nullptr || grandparent == nullptr) {
                return;
            }

            rb_node ** uncle = find_uncle(path, current_node);
            // uncle might be a null terminated leaf, thus its color will be black accordingly to rb tree properties
            bool uncle_color = (uncle == nullptr ? BLACK : (*uncle)->get_color());

            // perform balance if parent is red
            if ((*parent)->is_black()) {
                return;
            }

            if (uncle_color == RED) {
                (*parent)->change_color(BLACK);
                if (uncle != nullptr) {
                    (*uncle)->change_color(BLACK);
                }
                (*grandparent)->change_color(RED);

                // make path correct by popping current node's parent and grandparent out of it
                path.pop();
                path.pop();
                insertion_do_balance(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(grandparent));
            }
            else {
                if ((*grandparent)->left_subtree == (*parent)) {
                    // left-right case
                    if ((*parent)->right_subtree == (*current_node)) {
                        // do left rotation(current_node), make links correct, do left-left case
                        // actually, the rotation returns current node, but for left-right case, which consists of one rotation + left-left case, we pretend actual parent is newly inserted and actual inserted node is a parent; grandparent and uncle not changed
                        parent = reinterpret_cast<rb_node **>(this->rotate_left(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));
                        current_node = reinterpret_cast<rb_node **>(&((*parent)->left_subtree));
                    }
                    // left-left case
                    if ((*parent)->left_subtree == (*current_node)) {
                        // pop 1 value, do right_rotation(parent),push parent(wish), make parent black and grandparent red
                        path.pop();
                        parent = reinterpret_cast<rb_node **>(this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent)));
                        grandparent = reinterpret_cast<rb_node **>(&((*parent)->right_subtree));
//                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));

                        (*parent)->change_color(BLACK);
                        (*grandparent)->change_color(RED);
                    }
                }
                else {
                    // right-left case
                    if ((*parent)->left_subtree == (*current_node)) {
                        // do right_rotation(current_node), make links correct, do right-right case
                        // actually, the rotation returns current node, but for left-right case, which consists of one rotation + left-left case, we pretend actual parent is newly inserted and actual inserted node is a parent; grandparent and uncle not changed
                        parent = reinterpret_cast<rb_node **>(this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(current_node)));
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));
                        current_node = reinterpret_cast<rb_node **>(&((*parent)->right_subtree));
                    }
                    // right-right case
                    if ((*parent)->right_subtree == (*current_node)) {
                        // pop 1 value, do left_rotation(parent), push parent(wish), make parent black and grandparent red
                        path.pop();
                        parent = reinterpret_cast<rb_node **>(this->rotate_left(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent)));
                        grandparent = reinterpret_cast<rb_node **>(&((*parent)->left_subtree));
//                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));

                        (*parent)->change_color(BLACK);
                        (*grandparent)->change_color(RED);
                    }
                }
            }
        }

        void after_insert_inner(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename bs_tree<tkey, tvalue, tkey_comparer>::node **target_ptr) override
        {
            this->trace_with_guard("rb_tree::insertion_rb_tree::after_insert_inner method started");
            rb_node ** current_node = reinterpret_cast<rb_node **>(target_ptr);
            (*current_node)->change_color(RED);

            this->trace_with_guard("rb_tree::insertion_rb_tree::insertion_do_balance method started");
            insertion_do_balance(path, target_ptr);
            this->trace_with_guard("rb_tree::insertion_rb_tree::insertion_do_balance method finished")
                ->trace_with_guard("rb_tree::insertion_rb_tree::after_insert_inner method finished");
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
            bool first_color = first->get_color();
            first->change_color(second->get_color());
            second->change_color(first_color);
        }

        tvalue remove(tkey const &key) override
        {
            this->trace_with_guard("rb_tree::removing_rb_tree::remove method started");

            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            rb_node **target_ptr = reinterpret_cast<rb_node **>(path_and_target.second);

            if (*target_ptr == nullptr)
            {
                this->debug_with_guard("rb_tree::removing_rb_tree::remove no value with passed key in tree")
                    ->trace_with_guard("rb_tree::removing_rb_tree::remove method finished");
                throw typename bs_tree<tkey, tvalue, tkey_comparer>::remove_exception   ("rb_tree::removing_rb_tree::remove::no value with passed key in tree");
            }

            tvalue result = (*target_ptr)->value;
            bool target_ptr_color = (*target_ptr)->get_color();

            // deleting element with 2 children (color does not matter)
            if ((*target_ptr)->left_subtree != nullptr &&
                (*target_ptr)->right_subtree != nullptr)
            {
                if (target_ptr_color == RED) {
                    this->debug_with_guard("rb_tree::removing_rb_tree::remove deleting an RED element with 2 children");
                } else {
                    this->debug_with_guard("rb_tree::removing_rb_tree::remove deleting an BLACK element with 2 children");
                }

                typename bs_tree<tkey, tvalue, tkey_comparer>::node **element_to_swap_with = &((*target_ptr)->left_subtree);
                path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));

                bool element_to_swap_with_has_non_null_right_subtree = false;
                std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> swap_stack;

                if ((*element_to_swap_with)->right_subtree != nullptr) {
                    element_to_swap_with_has_non_null_right_subtree = true;
                    path.push(element_to_swap_with);
                    element_to_swap_with = &((*element_to_swap_with)->right_subtree);

                    bool local_element_to_swap_with_has_non_null_right_subtree = false;
                    do
                    {
                        if ((*element_to_swap_with)->right_subtree != nullptr) {
                            local_element_to_swap_with_has_non_null_right_subtree = true;
                            swap_stack.push(element_to_swap_with);
                            element_to_swap_with = &((*element_to_swap_with)->right_subtree);
                        } else {
                            local_element_to_swap_with_has_non_null_right_subtree = false;
                        }
                    } while (local_element_to_swap_with_has_non_null_right_subtree);
                }

                target_ptr = reinterpret_cast<rb_node **>(this->swap_nodes(element_to_swap_with, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr)));

                if (element_to_swap_with_has_non_null_right_subtree) {
                    path.pop();
                    path.push(&((*(path.top()))->left_subtree));

                    std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> reverse_swap_stack;
                    while (!(swap_stack.empty())) {
                        reverse_swap_stack.push(swap_stack.top());
                        swap_stack.pop();
                    }

                    while (!(reverse_swap_stack.empty())) {
                        path.push(reverse_swap_stack.top());
                        reverse_swap_stack.pop();
                    }
                }
            }

            // deleting an element with no children
            if ((*target_ptr)->left_subtree == nullptr && (*target_ptr)->right_subtree == nullptr)
            {
                this->debug_with_guard("rb_tree::removing_rb_tree::remove deleting an element with no children");
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                if (target_ptr_color == BLACK) {
                    after_remove(path);
                    if (this->get_root_node() != nullptr) {
                        reinterpret_cast<rb_node *>(this->get_root_node())->change_color(BLACK);
                    }
                }
            }
            // deleting an element with 1 child. There cannot be a red element to delete with one child
            else if ((*target_ptr)->left_subtree != nullptr)
            {
                this->debug_with_guard("rb_tree::removing_rb_tree::remove deleting an element with 1 left child");
                auto *target_left_subtree = (*target_ptr)->left_subtree;
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                *target_ptr = reinterpret_cast<rb_node *>(target_left_subtree);
                (*target_ptr)->change_color(BLACK);
            }
            else
            {
                this->debug_with_guard("rb_tree::removing_rb_tree::remove deleting an element with 1 right child");
                auto *target_right_subtree = (*target_ptr)->right_subtree;
                this->cleanup_node(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(target_ptr));
                *target_ptr = reinterpret_cast<rb_node *>(target_right_subtree);
                (*target_ptr)->change_color(BLACK);
            }

            this->trace_with_guard("rb_tree::removing_template_method::remove method finished");
            return result;
        }

        void after_remove(std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **> &path) const override
        {
            this->trace_with_guard("rb_tree::removing_rb_tree::after_remove method started");
            // path содержит все узлы до удалённого, верхний узел в стеке -- родитель удалённого
            // warning! color of root may change. Change color if needed
            if (path.empty()) {
                return;
            }
            rb_node ** parent = reinterpret_cast<rb_node **>(path.top());
            rb_node ** brother_to_deleted =
                    reinterpret_cast<rb_node **>(((*parent)->left_subtree == nullptr ? &((*parent)->right_subtree) : &((*parent)->left_subtree)));

            if ((*brother_to_deleted) == nullptr) {
                return;
            }

            rb_node **brother_right_child, **brother_left_child;
            bool brother_right_child_color, brother_left_child_color;

            // deleted node was a left child of parent
            if ((*parent)->left_subtree == nullptr) {
                // case 1) if brother is red, do rotation(brother, parent), make parent red and brother black. redo links: brother has changed, so find new brother. Go to next case (brother is now black)
                if ((*brother_to_deleted)->is_red()) {
                    brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_left(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted)));
                    parent = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                    (*parent)->change_color(RED);
                    (*brother_to_deleted)->change_color(BLACK);
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));
                    brother_to_deleted = reinterpret_cast<rb_node **>(&((*parent)->right_subtree));
                }

                // case 2) if brother is black we have 3 sub-cases:
                // for another cases we need to find brother's children and their colors
                if ((*brother_to_deleted) != nullptr) {
                    brother_left_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                    brother_right_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                    brother_left_child_color = (*brother_left_child)->get_color();
                    brother_right_child_color = (*brother_right_child)->get_color();
                } else {
                    brother_left_child_color = BLACK;
                    brother_right_child_color = BLACK;
                }

                // case 2.1) children of brother are both black. Make brother red and parent black.
                if (brother_left_child_color == BLACK && brother_right_child_color == BLACK) {
                    if ((*brother_to_deleted) != nullptr) {
                        (*brother_to_deleted)->change_color(RED);
                    }
                    (*parent)->change_color(BLACK);
                }
                else {
                    // case 2.2) brother's right child is black and left child is red. Do rotation(brother, his left child), make brother red, his left child black. Go to next case with correct links
                    if (brother_left_child_color == RED) {
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                        brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_left_child)));
                        (*brother_to_deleted)->change_color(BLACK);
                        reinterpret_cast<rb_node *>((*brother_to_deleted)->right_subtree)->change_color(RED);
                        brother_left_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                        brother_right_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                        brother_left_child_color = (*brother_left_child)->get_color();
                        brother_right_child_color = RED;
                    }

                    // case 2.3) (deleted node is left child) brother's right child is red. Make brother be of color of our parent, brother's right child, our parent black. Do rotation (parent, brother)
                    if (brother_right_child_color == RED) {
                        bool parent_color = (*parent)->get_color();
                        brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_left(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted)));
                        parent = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                        (*brother_to_deleted)->change_color(parent_color);
                        (*brother_right_child)->change_color(BLACK);
                        (*parent)->change_color(BLACK);
                    }
                }
            }
            // cases are simply mirrored if deleted node was a right child
            else {
                // case 1) if brother is red, do rotation(brother, parent), make parent red and brother black. redo links: brother has changed, so find new brother. Go to next case (brother is now black)
                if ((*brother_to_deleted)->is_red()) {
                    brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted)));
                    parent = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                    (*parent)->change_color(RED);
                    (*brother_to_deleted)->change_color(BLACK);
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                    path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(parent));
                    brother_to_deleted = reinterpret_cast<rb_node **>(&((*parent)->left_subtree));
                }

                // case 2) if brother is black we have 3 sub-cases:
                // for another cases we need to find brother's children and their colors
                if ((*brother_to_deleted) != nullptr) {
                    brother_left_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                    brother_right_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                    brother_left_child_color = (*brother_left_child)->get_color();
                    brother_right_child_color = (*brother_right_child)->get_color();
                } else {
                    brother_left_child_color = BLACK;
                    brother_right_child_color = BLACK;
                }

                // case 2.1) children of brother are both black. Make brother red and parent black.
                if (brother_left_child_color == BLACK && brother_right_child_color == BLACK) {
                    if ((*brother_to_deleted) != nullptr) {
                        (*brother_to_deleted)->change_color(RED);
                    }
                    (*parent)->change_color(BLACK);
                }
                else {
                    // case 2.2) brother's right child is black and left child is red. Do rotation(brother, his left child), make brother red, his left child black. Go to next case with correct links
                    if (brother_right_child_color == RED) {
                        path.push(reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted));
                        brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_left(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_right_child)));
                        (*brother_to_deleted)->change_color(BLACK);
                        reinterpret_cast<rb_node *>((*brother_to_deleted)->left_subtree)->change_color(RED);
                        brother_left_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->left_subtree));
                        brother_right_child = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                        brother_left_child_color = RED;
                        brother_right_child_color = (*brother_right_child)->get_color();
                    }

                    // case 2.3) (deleted node is left child) brother's right child is red. Make brother be of color of our parent, brother's right child, our parent black. Do rotation (parent, brother)
                    if (brother_left_child_color == RED) {
                        bool parent_color = (*parent)->get_color();
                        brother_to_deleted = reinterpret_cast<rb_node **>(this->rotate_right(path, reinterpret_cast<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>(brother_to_deleted)));
                        parent = reinterpret_cast<rb_node **>(&((*brother_to_deleted)->right_subtree));
                        (*brother_to_deleted)->change_color(parent_color);
                        (*brother_left_child)->change_color(BLACK);
                        (*parent)->change_color(BLACK);
                    }
                }
            }
            this->trace_with_guard("rb_tree::removing_rb_tree::after_remove method finished");
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

private:
    // constructor
    rb_tree(
            logger *logger,
            memory *allocator,
            insertion_rb_tree *insertion,
            typename bs_tree<tkey, tvalue, tkey_comparer>::finding_template_method *finding,
            removing_rb_tree *removing)
    {
        this->_logger(logger);
        this->_allocator(allocator);
        this->_insertion(insertion);
        this->_finding(finding);
        this->_removing(removing);
        this->_root(nullptr);

        this->trace_with_guard("rb_tree constructor was called");
    }

public:
    // copy constructor
    rb_tree(rb_tree<tkey, tvalue, tkey_comparer> const &obj)
    : rb_tree(obj._logger, obj._allocator)
    {
        this->trace_with_guard("rb_tree copy constructor was called");
        this->_root = this->copy(obj._root);
    }

    // move constructor
    rb_tree(rb_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    : rb_tree(obj._insertion,
              obj._finding,
              obj._removing,
              obj._allocator,
              obj._logger)
    {
        this->trace_with_guard("rb_tree move constructor was called");

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
    rb_tree &operator=(rb_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        this->trace_with_guard("rb_tree copy assignment constructor was called");

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
    rb_tree &operator=(rb_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        this->trace_with_guard("rb_tree move assignment constructor was called");

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
    ~rb_tree()
    {
        this->trace_with_guard("rb_tree destructor was called");

        /*
        delete this->_insertion;
        delete this->_finding;
        delete this->_removing;

        this->clearup(this->_root);
        */
     }

private:
    void clearup(typename bs_tree<tkey, tvalue, tkey_comparer>::node *element) override
    {
        if (element == nullptr)
        {
            return;
        }

        clearup(element->left_subtree);
        clearup(element->right_subtree);

        reinterpret_cast<rb_node *>(element)->~rb_node();
        this->deallocate_with_guard(element);
    }

    typename bs_tree<tkey, tvalue, tkey_comparer>::node *copy(typename bs_tree<tkey, tvalue, tkey_comparer>::node *from) override
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        rb_node *result = reinterpret_cast<rb_node *>(this->allocate_with_guard(sizeof(rb_node)));
        new (result) rb_node(*reinterpret_cast<rb_node *>(from));
        result->change_color(reinterpret_cast<rb_node *>(from)->get_color());

        result->left_subtree = copy(from->left_subtree);
        result->right_subtree = copy(from->right_subtree);

        return result;
    }
};


#endif //RB_TREE_H
