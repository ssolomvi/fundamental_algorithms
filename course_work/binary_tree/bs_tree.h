#ifndef BS_TREE_H
#define BS_TREE_H

#include <stack>
#include <utility>
#include "associative_container.h"
#include "../logger/logger.h"
#include "../logger/logger_holder.h"
#include "../allocator/memory_base_class.h"
#include "../allocator/memory_holder.h"

template<typename tkey, typename tvalue, typename tkey_comparer>
class bs_tree:
        public associative_container<tkey, tvalue>,
        protected memory_holder,
        protected logger_holder
{

public:

    struct node
    {
        tkey key;
        tvalue value;
        node *left_subtree;
        node *right_subtree;

    public:

        node() = default;

        virtual ~node() noexcept = default;

    };

public:
    class insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit insert_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };
public:
    class find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit find_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit remove_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class iterator_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit iterator_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

#pragma region iterators
#pragma region preorder iterator
public:
    class prefix_iterator final
    {
    friend class bs_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;

    public:
        explicit prefix_iterator(node *current_node)
        {
            this->_current_node = current_node;
        }

        bool operator==(prefix_iterator const &other) const
        {
            return (_current_node == other._current_node);
        }

        bool operator!=(prefix_iterator const &other) const
        {
            return (_current_node != other._current_node);
        }

        prefix_iterator& operator++()
        {
            if (_current_node == nullptr) {
                throw iterator_exception("iterator iterator is out of range");
            }

            if (_current_node->left_subtree != nullptr) {
                _path.push(_current_node);
                _current_node = _current_node->left_subtree;
            }
            else if (_current_node->right_subtree != nullptr) {
                _path.push(_current_node);
                _current_node = _current_node->right_subtree;
            }
            else {
                if (_path.empty() == true) {
                    _current_node = nullptr;
                }
                else if (_path.top()->left_subtree == _current_node) {
                    while (true) {
                        if (_path.empty() == true) {
                            _current_node = nullptr;
                            break;
                        }

                        // parent element has a right subtree
                        if (_path.top()->right_subtree != nullptr) {
                            _current_node = _path.top()->right_subtree;
                            break;
                        } else {
                            while (true) {
                                _current_node = _path.top();
                                _path.pop();

                                if (_path.empty() == true) {
                                    _current_node = nullptr;
                                    break;
                                }

                                if (_path.top()->left_subtree == _current_node && _path.top()->right_subtree != nullptr) {
                                    _current_node = _path.top()->right_subtree;
                                    break;
                                }
                            }
                        }

                    }
                }
                else if (_path.top()->right_subtree == _current_node) {
                    while (true) {
                        _current_node = _path.top();
                        _path.pop();

                        if (_path.empty() == true) {
                            _current_node = nullptr;
                            break;
                        }

                        if (_path.top()->left_subtree == _current_node && _path.top()->right_subtree != nullptr) {
                            _current_node = _path.top()->right_subtree;
                            break;
                        }
                    }
                }
            }

            return *this;
        }

        prefix_iterator operator++(int not_used)
        {
            auto result = *this;
            ++*this;
            return *this;
        }

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const
        {
            return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
        }

    };
#pragma endregion

#pragma region inorder iterator
public:
    class infix_iterator final
    {
        friend class bs_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;

    public:
        explicit infix_iterator(node *current_node)
                : _current_node(current_node)
        {
            if (_current_node == nullptr) {
                return;
            }

            while (_current_node->left_subtree != nullptr) {
                _path.push(_current_node);
                _current_node = _current_node->left_subtree;
            }
        }

        bool operator==(infix_iterator const &other) const
        {
            return (_current_node == other._current_node);
        }

        bool operator!=(infix_iterator const &other) const
        {
            return (_current_node != other._current_node);
        }

        infix_iterator& operator++()
        {
            // левое--корень--правое
            if (_current_node == nullptr) {
                throw iterator_exception("infix_iterator iterator is out of range");
            }

            if (_current_node->right_subtree != nullptr) {
                _path.push(_current_node);
                _current_node = _current_node->right_subtree;
                while (_current_node->left_subtree != nullptr) {
                    _path.push(_current_node);
                    _current_node = _current_node->left_subtree;
                }
            }
            else if (_path.top()->left_subtree == _current_node) {
                _current_node = _path.top();
                _path.pop();
            }
            else if (_path.top()->right_subtree == _current_node) {
                while (_path.top()->right_subtree == _current_node) {
                    _current_node = _path.top();
                    _path.pop();

                    if (_path.empty()) {
                        _current_node = nullptr;
                        break;
                    }
                }

                if (!_path.empty()) {
                    _current_node = _path.top();
                    _path.pop();
                }
            }

            return *this;
        }

        infix_iterator operator++(int not_used)
        {
            auto result = *this;
            ++*this;
            return *this;
        }

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const
        {
            return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
        }
    };

#pragma endregion

#pragma region postorder iterator
public:
    class postfix_iterator final
    {
        friend class bs_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;
    public:
        explicit postfix_iterator(node *current_node)
                : _current_node(current_node)
        {
            // лево-право-текущий
            if (_current_node == nullptr) {
                return;
            }

            while (_current_node->right_subtree || _current_node->left_subtree) {
                _path.push(_current_node);
                _current_node = _current_node->left_subtree != nullptr ?
                                _current_node->left_subtree :
                                _current_node->right_subtree;
            }
        }

        bool operator==(postfix_iterator const &other) const
        {
            return (_current_node == other._current_node);
        }

        bool operator!=(postfix_iterator const &other) const
        {
            return (_current_node != other._current_node);
        }

        postfix_iterator &operator++()
        {
            // если можно идти налево, идём, если можно направо, идём
            if (_current_node == nullptr) {
                throw iterator_exception("postfix_iterator iterator is out of range");
            }

            if (_path.empty() == true) {
                _current_node = nullptr;
            }
                // поднимаемся слева
            else if (_path.top()->left_subtree == _current_node) {
                if (_path.top()->right_subtree != nullptr) {
                    _current_node = _path.top()->right_subtree;
                    while (_current_node->right_subtree || _current_node->left_subtree) {
                        _path.push(_current_node);
                        _current_node = _current_node->left_subtree != nullptr ?
                                        _current_node->left_subtree :
                                        _current_node->right_subtree;
                    }
                } else {
                    _current_node = _path.top();
                    _path.pop();
                }
            }
                // поднимаемся справа
            else {
                _current_node = _path.top();
                _path.pop();
            }
            return *this;
        }

        postfix_iterator operator++(int not_used)
        {
            auto result = *this;
            ++*this;
            return *this;
        }

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const
        {
            return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
        }

    };
#pragma endregion

public:
    prefix_iterator begin_prefix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::prefix_iterator(_root);
    }

    prefix_iterator end_prefix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::prefix_iterator(nullptr);
    }

    infix_iterator begin_infix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::infix_iterator(_root);
    }

    infix_iterator end_infix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::infix_iterator(nullptr);
    }

    postfix_iterator begin_postfix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::postfix_iterator(_root);
    }

    postfix_iterator end_postfix() const noexcept
    {
        return bs_tree<tkey, tvalue, tkey_comparer>::postfix_iterator(nullptr);
    }

#pragma endregion

protected:
#pragma region template methods
    class template_method_basics:
            protected logger_holder
    {
        friend class bs_tree<tkey, tvalue, tkey_comparer>;

    protected:

    bs_tree<tkey, tvalue, tkey_comparer> *_target_tree;
    node * get_root_node()
    {
        return _target_tree->_root;
    }

    public:

        explicit template_method_basics(
                bs_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : _target_tree(target_tree)
        {

        }

    public:

        std::pair<std::stack<node **>, node **> find_path(tkey const &key) const
        {
            std::stack<node **> path;

            if (_target_tree->_root == nullptr)
            {
                return { path, &_target_tree->_root };
            }

            auto **iterator = &_target_tree->_root;
            tkey_comparer comparer;

            while ((*iterator) != nullptr)
            {
                auto comparison_result = comparer(key, (*iterator)->key);
                if (comparison_result == 0)
                {
                    return std::pair<std::stack<typename bs_tree<tkey, tvalue, tkey_comparer>::node **>,
                            typename bs_tree<tkey, tvalue, tkey_comparer>::node**>(path, iterator);
                }

                path.push(iterator);
                iterator = comparison_result < 0
                           ? &(*iterator)->left_subtree
                           : &(*iterator)->right_subtree;
            }

            return { path, iterator };
        }

        node** find_parent(std::stack<node **> &path, node **target_ptr) const
        {
            return path.empty() ? nullptr : path.top();
        }

        node** find_grandparent(std::stack<node **> &path, node **target_ptr) const
        {
            if (path.empty())
                return nullptr;
            auto ** parent = path.top();
            path.pop();
            if (path.empty()) {
                path.push(parent);
                return nullptr;
            }
            auto ** grandparent = path.top();
            path.push(parent);
            return grandparent;
        }

    public:
        virtual void rotate_fix_additional_data(node * target_ptr) const
        {

        }

    public:
        node** rotate_left(std::stack<node **> &path, node **target_ptr) const
        {
            node ** parent = find_parent(path, target_ptr);
            path.pop();

            node * left_to_target_ptr = (*target_ptr)->left_subtree;

            (*target_ptr)->left_subtree = (*parent);
            (*parent) = (*target_ptr);

            (*target_ptr)->left_subtree->right_subtree = left_to_target_ptr;

            this->rotate_fix_additional_data(*parent);
            return parent;
        }

        node** rotate_right(std::stack<node **> &path, node **target_ptr) const
        {
            node ** parent = find_parent(path, target_ptr);
            path.pop();

            node * right_to_target_ptr = (*target_ptr)->right_subtree;

            (*target_ptr)->right_subtree = (*parent);
            (*parent) = (*target_ptr);

            (*target_ptr)->right_subtree->left_subtree = right_to_target_ptr;

            this->rotate_fix_additional_data(*parent);
            return parent;
        }

    private:

        [[nodiscard]] logger *get_logger() const noexcept override
        {
            return _target_tree->get_logger();
        }

    };

#pragma region insertion template method
    class insertion_template_method :
            public template_method_basics,
            private memory_holder
    {

    public:

        explicit insertion_template_method(
                bs_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }

    public:
        void insert(tkey const &key, tvalue &&value)
        {
            this->trace_with_guard("bs_tree::insertion_template_method::insert method started");
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr != nullptr)
            {
                this->debug_with_guard("bs_tree::insertion_template_method::insert passed key is not unique")
                    ->trace_with_guard("bs_tree::insertion_template_method::insert method finished");
                throw insert_exception("bs_tree::insertion_template_method::insert passed key is not unique");
            }

            *target_ptr = reinterpret_cast<node *>(allocate_with_guard(get_node_size()));
            initialize_memory_with_node(*target_ptr);

            (*target_ptr)->key = key;
            (*target_ptr)->value = std::move(value);
            (*target_ptr)->left_subtree = nullptr;
            (*target_ptr)->right_subtree = nullptr;

            after_insert_inner(path, target_ptr);
            this->trace_with_guard("bs_tree::insertion_template_method::insert method finished");
        }

        // pass old key, new key, new value
        void update(tkey const &key, tkey const & new_key, tvalue &&value)
        {
            this->trace_with_guard("bs_tree::insertion_template_method::update method started");
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr == nullptr)
            {
                this->debug_with_guard("bs_tree::insertion_template_method::update passed key is not in collection")
                        ->trace_with_guard("bs_tree::insertion_template_method::update method finished");
                throw insert_exception("bs_tree::insertion_template_method::update passed key is not in collection");
            }

            path_and_target = this->find_path(new_key);
            path = path_and_target.first;
            target_ptr = path_and_target.second;

            *target_ptr = reinterpret_cast<node *>(allocate_with_guard(get_node_size()));
            initialize_memory_with_node(*target_ptr);

            (*target_ptr)->key = new_key;
            (*target_ptr)->value = std::move(value);
            (*target_ptr)->left_subtree = nullptr;
            (*target_ptr)->right_subtree = nullptr;

            after_insert_inner(path, target_ptr);
            this->trace_with_guard("bs_tree::insertion_template_method::update method finished");
        }

    protected:

        [[nodiscard]] virtual size_t get_node_size() const
        {
            return sizeof(node);
        }

        virtual void initialize_memory_with_node(
                node *target_ptr) const
        {
            new(target_ptr) node;
        }

        virtual void after_insert_inner(
                std::stack<node **> &path,
                node **target_ptr)
        {

        }

    private:

        [[nodiscard]] memory *get_memory() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    };
#pragma endregion

#pragma region finding template method
    class finding_template_method :
            public template_method_basics
    {

    public:
        explicit finding_template_method(bs_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }

    public:
        tvalue const &find(tkey const &key)
        {
            this->trace_with_guard("bs_tree::finding_template_method::find method started");
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr == nullptr)
            {
                this->debug_with_guard("bs_tree::finding_template_method::find no value with passed key in tree")
                    ->trace_with_guard("bs_tree::finding_template_method::find method finished");
                throw find_exception("bs_tree::finding_template_method::find no value with passed key in tree");
            }

            after_find_inner(path, target_ptr);

            this->trace_with_guard("bs_tree::finding_template_method::find method finished");
            return (*target_ptr)->value;
        }

    protected:

        virtual void after_find_inner(
                std::stack<node **> &path,
                node **&target_ptr)
        {
            // TODO: nothing to do here in BST context...
        }

    };

#pragma endregion


#pragma region removing template method
    class removing_template_method:
            public template_method_basics,
            private memory_holder
    {

    public:

        explicit removing_template_method(
                bs_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }

    public:

        virtual tvalue remove(tkey const &key)
        {
            this->trace_with_guard("bs_tree::removing_template_method::remove method started");
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            auto **target_ptr = path_and_target.second;

            if (*target_ptr == nullptr)
            {
                this->debug_with_guard("bs_tree::removing_template_method::remove no value with passed key in tree")
                    ->trace_with_guard("bs_tree::removing_template_method::remove method finished");
                throw remove_exception("bs_tree::removing_template_method::remove no value with passed key in tree");
            }

            // здесь могла быть move-семантика
            tvalue result = (*target_ptr)->value;

            if ((*target_ptr)->left_subtree != nullptr &&
                (*target_ptr)->right_subtree != nullptr)
            {
                this->debug_with_guard("bs_tree::removing_template_method::remove deleting an element with 2 children");
                auto **element_to_swap_with = &(*target_ptr)->left_subtree;
                path.push(target_ptr);

                bool element_to_swap_with_has_non_null_right_subtree = false;
                std::stack<node **> swap_stack;

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

                target_ptr = swap_nodes(element_to_swap_with, target_ptr);

                if (element_to_swap_with_has_non_null_right_subtree) {
                    path.pop();
                    path.push(&((*(path.top()))->left_subtree));

                    std::stack<node **> reverse_swap_stack;
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

            if ((*target_ptr)->left_subtree == nullptr &&
                (*target_ptr)->right_subtree == nullptr)
            {
                this->debug_with_guard("bs_tree::removing_template_method::remove deleting an element with no children");
                cleanup_node(target_ptr);
            }
            else if ((*target_ptr)->left_subtree != nullptr)
            {
                this->debug_with_guard("bs_tree::removing_template_method::remove deleting an element with 1 left child");
                auto *target_left_subtree = (*target_ptr)->left_subtree;
                cleanup_node(target_ptr);
                *target_ptr = target_left_subtree;
            }
            else
            {
                this->debug_with_guard("bs_tree::removing_template_method::remove deleting an element with 1 right child");
                auto *target_right_subtree = (*target_ptr)->right_subtree;
                cleanup_node(target_ptr);
                *target_ptr = target_right_subtree;
            }

            if (!(path.empty())) {
                after_remove(path);
            }

            this->trace_with_guard("bs_tree::removing_template_method::remove method finished");
            return result;
        }

    protected:

        template<typename T>
        void swap(
                T **left,
                T **right)
        {
            T *temp = *left;
            *left = *right;
            *right = temp;
        }

        node** swap_nodes(
                node **one_node,
                node **another_node)
        {
            // близкородственный свап до хорошего не доводит
            if ((*another_node)->left_subtree == (*one_node)) {
                node * grandfather = (*another_node);
                node * grandchild = (*one_node)->left_subtree;
                (*another_node) = (*one_node);
                (*another_node)->left_subtree = grandfather;
                (*another_node)->right_subtree = grandfather->right_subtree;
                (*another_node)->left_subtree->left_subtree = grandchild;
                (*another_node)->left_subtree->right_subtree = nullptr;
                return (&(*another_node)->left_subtree);
            } else {
                swap(&((*one_node)->left_subtree), &((*another_node)->left_subtree));
                swap(&((*one_node)->right_subtree), &((*another_node)->right_subtree));

                swap_additional_data(*one_node, *another_node);

                swap(one_node, another_node);
                return one_node;
            }
        }

        void cleanup_node(
                node **node_address)
        {
            (*node_address)->~node();
            deallocate_with_guard(reinterpret_cast<void *>(*node_address));

            *node_address = nullptr;
        }

    protected:

        virtual void swap_additional_data(
                node *one_node,
                node *another_node)
        {

        }

        virtual void after_remove(
                std::stack<node **> &path) const
        {

        }

    private:

        memory *get_memory() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    };

#pragma endregion
#pragma endregion

    node *_root;
protected:
#pragma region rool 5
    // constructor
    bs_tree(
            logger *logger,
            memory *allocator,
            insertion_template_method *insertion,
            finding_template_method *finding,
            removing_template_method *removing)
            : _logger(logger),
              _allocator(allocator),
              _insertion(insertion),
              _finding(finding),
              _removing(removing),
              _root(nullptr)
    {
        this->trace_with_guard("bs_tree constructor was called");
    }

protected:
    logger *_logger;
    memory *_allocator;
    insertion_template_method *_insertion;
    finding_template_method *_finding;
    removing_template_method *_removing;

public:
    // copy constructor
    bs_tree(bs_tree<tkey, tvalue, tkey_comparer> const &obj)
            : bs_tree(obj._logger, obj._allocator)
    {
        this->trace_with_guard("bs_tree copy constructor was called");
        _root = copy(obj._root);
    }

    // move constructor
    bs_tree(bs_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
            : bs_tree(obj._insertion,
                      obj._finding,
                      obj._removing,
                      obj._allocator,
                      obj._logger)
    {
        this->trace_with_guard("bs_tree move constructor was called");
        _root = obj._root;
        obj._root = nullptr;

        _insertion->_target_tree = this;
        obj._insertion = nullptr;

        _finding->_target_tree = this;
        obj._finding = nullptr;

        _removing->_target_tree = this;
        obj._removing = nullptr;

        obj._allocator = nullptr;

        obj._logger = nullptr;
    }

    // copy assignment (оператор присваивания)
    bs_tree &operator=(bs_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        this->trace_with_guard("bs_tree copy assignment constructor was called");
        if (this == &obj)
        {
            return *this;
        }

        clearup(_root);

        _allocator = obj._allocator;
        _logger = obj._logger;

        _root = copy(obj._root);

        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    bs_tree &operator=(bs_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        this->trace_with_guard("bs_tree move assignment constructor was called");
        if (this == &obj)
        {
            return *this;
        }

        clearup(_root);
        _root = obj._root;
        obj._root = nullptr;

        delete obj._insertion;
        obj._insertion = nullptr;

        delete obj._finding;
        obj._finding = nullptr;

        delete obj._removing;
        obj._removing = nullptr;

        _allocator = obj._allocator;
        obj._allocator = nullptr;

        _logger = obj._logger;
        obj._logger = nullptr;

        return *this;
    }

    // destructor
    ~bs_tree()
    {
        this->trace_with_guard("bs_tree destructor was called");
        delete _insertion;
        delete _finding;
        delete _removing;

        clearup(_root);
    }

protected:
    virtual void clearup(node *element)
    {
        if (element == nullptr)
        {
            return;
        }

        clearup(element->left_subtree);
        clearup(element->right_subtree);

        element->~node();
        deallocate_with_guard(element);
    }

    virtual node *copy(node *from)
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        node *result = reinterpret_cast<node *>(allocate_with_guard(sizeof(node)));
        new (result) node(*from);

        result->left_subtree = copy(from->left_subtree);
        result->right_subtree = copy(from->right_subtree);

        return result;
    }

public:
    // constructor
    explicit bs_tree(
            logger *logger = nullptr,
            memory *allocator = nullptr)
            : bs_tree(logger,
                      allocator,
                      new insertion_template_method(this),
                      new finding_template_method(this),
                      new removing_template_method(this))
    {

    }
#pragma endregion

public:

    void insert(
            tkey const &key,
            tvalue &&value) override
    {
        _insertion->insert(key, std::move(value));
    }

    /*
    void update_value(key const &key, key const &new_key, tvalue &&value) override
    {
        _insertion->update(key, new_key, std::move(value));
    }
     */
    tvalue const &get(
            tkey const &key) override
    {
        return _finding->find(key);
    }

    tvalue remove(
            tkey const &key) override
    {
        return _removing->remove(key);
    }

private:

    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return _allocator;
    }

private:

    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return _logger;
    }

};

#endif //BS_TREE_H
