#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <stack>
#include "associative_container.h"
#include "../logger/logger.h"
#include "../logger/logger_holder.h"
#include "../allocator/memory_base_class.h"
#include "../allocator/memory_holder.h"

// TODO: rotations + close todos + throw all exceptions + do tests + debug iterators + copy
// TODO: do avl, rb, splay =)

template<typename tkey, typename tvalue, typename tkey_comparer>
class binary_search_tree  :
        public associative_container<tkey, tvalue>,
        private memory_holder,
        private logger_holder
{
public:
    struct node
    {
        tkey key;
        tvalue value;
        node *left_subtree;
        node *right_subtree;

    public:
        virtual ~node() noexcept = default;
    };

public:
    class bst_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit bst_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }

    };

#pragma region iterators
#pragma region preorder iterator
public:
    class prefix_iterator final
    {
//        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;

    public:
        explicit prefix_iterator(node *current_node);

        bool operator==(prefix_iterator const &other) const;
        bool operator!=(prefix_iterator const &other) const;

        prefix_iterator& operator++();
        prefix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;

//    private:
//        [[nodiscard]] logger *get_logger() const noexcept override;
    };
#pragma endregion

#pragma region inorder iterator
public:
    class infix_iterator final
    {
        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;

    public:
        explicit infix_iterator(node *current_node);

        bool operator==(infix_iterator const &other) const;
        bool operator!=(infix_iterator const &other) const;

        infix_iterator& operator++();
        infix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;

//    private:
//        [[nodiscard]] logger *get_logger() const noexcept override;
    };

#pragma endregion

#pragma region postorder iterator
public:
    class postfix_iterator final
    {
        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<node *> _path;
        binary_search_tree<tkey, tvalue, tkey_comparer> const *_iterable_context;
    public:
        explicit postfix_iterator(node *current_node);

        bool operator==(postfix_iterator const &other) const;
        bool operator!=(postfix_iterator const &other) const;

        postfix_iterator &operator++();
        postfix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue const&> operator*() const;

//    private:
//        [[nodiscard]] logger *get_logger() const noexcept override;
    };
#pragma endregion

public:
    binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator begin_prefix() const noexcept;

    binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator end_prefix() const noexcept;

    binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator begin_infix() const noexcept;

    binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator end_infix() const noexcept;

    binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator begin_postfix() const noexcept;

    binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator end_postfix() const noexcept;

#pragma endregion

protected:
#pragma region template methods
    class template_method_basics: protected logger_holder
    {
        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    protected:
        binary_search_tree<tkey, tvalue, tkey_comparer> *_target_tree;

    public:
        explicit template_method_basics(binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree);

        virtual ~template_method_basics() noexcept = default;

    public:

        std::pair<std::stack<node **>, node **> find_path(tkey const &key);

        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** find_parent(std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **target_ptr);

        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node** find_grandparent(std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **target_ptr);

        void rotate_left(std::stack<node **> &path, node **target_ptr);

        void rotate_right(std::stack<node **> &path, node **target_ptr);

    private:

        logger *get_logger() const noexcept override;

    };


#pragma region insertion template method
    class insertion_template_method :
            public template_method_basics,
            private memory_holder
    {

    public:

        explicit insertion_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree);

    public:

        void insert(tkey const &key, tvalue &&value);

    protected:

        [[nodiscard]] virtual size_t get_node_size() const;

        virtual void initialize_memory_with_node(node *target_ptr) const;

        virtual void after_insert_inner(std::stack<node **> &path, node **target_ptr);

    private:

        memory *get_memory() const noexcept override;

    };
#pragma endregion

#pragma region finding template method
    class finding_template_method : public template_method_basics
    {

    public:

        explicit finding_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree);

    public:
        tvalue const &find(tkey const &key);

    protected:

        virtual void after_find_inner(std::stack<node **> &path, node **target_ptr);

    };
#pragma endregion

#pragma region removing template method
    class removing_template_method:
            public template_method_basics,
            private memory_holder
    {

    public:

        explicit removing_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree);

    public:

        tvalue &&remove(tkey const &key);

    private:

        template<typename T>
        void swap(T **left, T **right);

        void swap_nodes(node **one_node, node **another_node);

        void cleanup_node(node **node_address);

    protected:

        virtual void swap_additional_data(node *one_node, node *another_node);

        virtual void after_remove(std::stack<node **> const &path);

    private:

        memory *get_memory() const noexcept override;

    };
#pragma endregion
#pragma endregion

private:
    node *_root;
    logger *_logger;
    memory *_allocator;
    insertion_template_method *_insertion;
    finding_template_method *_finding;
    removing_template_method *_removing;

public:
#pragma region rool 5
    // copy constructor
    binary_search_tree(
            binary_search_tree<tkey, tvalue, tkey_comparer> const &obj)
            : binary_search_tree(obj._logger, obj._allocator)
    {
        _root = copy(obj._root);
    }

    binary_search_tree(
            binary_search_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
            : binary_search_tree(obj._insertion,
                                 obj._finding,
                                 obj._removing,
                                 obj._allocator,
                                 obj._logger)
    {
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

    binary_search_tree &operator=(
            binary_search_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        clear_up(_root);

        _allocator = obj._allocator;
        _logger = obj._logger;

        _root = copy(obj._root);

        return *this;
    }

    binary_search_tree &operator=(
            binary_search_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
        if (this == &obj)
        {
            return *this;
        }

        clear_up(_root);
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

    ~binary_search_tree() override
    {
        delete _insertion;
        delete _finding;
        delete _removing;

        clear_up(_root);
    }

private:

    void clear_up(node *element);

    // TODO: think about usability in derived classes
    node *copy(node *from);

//    virtual void copy_additional_data();

protected:

    binary_search_tree(
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

    }

public:
//    explicit binary_search_tree(logger *logger, memory *allocator);
    explicit binary_search_tree(
            logger *logger = nullptr,
            memory *allocator = nullptr)
            : binary_search_tree(logger,
                              allocator,
                              new insertion_template_method(this),
                              new finding_template_method(this),
                              new removing_template_method(this))
    {

    }
#pragma endregion

public:
#pragma region associative container contract
    void insert(tkey const &key, tvalue const &&value) override;

    tvalue const &get(tkey const &key) override;

    tvalue &&remove(tkey const &key) override;

#pragma endregion

private:

    memory *get_memory() const noexcept override;

private:

    logger *get_logger() const noexcept override;
};



#endif //BINARY_SEARCH_TREE_H
