#ifndef B_TREE_H
#define B_TREE_H

#include <stack>
#include "../binary_tree/associative_container.h"
#include "../logger/logger.h"
#include "../logger/logger_holder.h"
#include "../allocator/memory_base_class.h"
#include "../allocator/memory_holder.h"

template <
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class b_tree :
        public associative_container<tkey, tvalue>,
        private memory_holder,
        private logger_holder
{
protected:
    // number of keys in nodes vary [t-1 ; 2t-1] excluding root where this number is (1 ; 2t-1]
    struct node {
        bool _is_leaf;
        unsigned _number_of_keys;
        tkey * _keys;
        tvalue * _values; // ??
        struct node ** _sub_nodes;
    };

    class insertion_template_method;
    class finding_template_method;
    class removing_template_method;

    unsigned _tree_parameter;
    node * _root;
    logger * _logger;
    memory * _allocator;
    insertion_template_method * _insertion;
    finding_template_method * _finding;
    removing_template_method * _removing;

#pragma region template methods
    // todo: do find_path
    class template_method_basics:
            protected logger_holder
    {
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    protected:

        b_tree<tkey, tvalue, tkey_comparer> *_target_tree;

        node *get_root_node() {
            return _target_tree->_root;
        }

        std::pair<std::stack<node **>, node **> find_path(tkey const &key) const
        {

        }

    private:
        logger *get_logger() const noexcept override
        {
            return _target_tree->get_logger();
        }

    public:
        explicit template_method_basics(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : _target_tree(target_tree)
        {
        }
    };

#pragma region insertion template method
    class insertion_template_method :
            public template_method_basics,
            private memory_holder
    {
    public:
        void insert(
                tkey const &key,
                tvalue &&value) {
        }

        [[nodiscard]] virtual size_t get_node_size() const
        {
            return sizeof(node);
        }

        virtual void initialize_memory_with_node(
                node *target_ptr) const
        {
            new(target_ptr) node;
        }

    private:

        [[nodiscard]] memory *get_memory() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    public:
        explicit insertion_template_method(
                b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }
    };
#pragma endregion

#pragma region finding template method
    class finding_template_method :
            public template_method_basics
    {
    public:
        tvalue const &find(
                tkey const &key)
        {

        }

    public:

        explicit finding_template_method(
                b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }
    };

#pragma endregion

#pragma region removing template method
    class removing_template_method:
            public template_method_basics,
            private memory_holder
    {
    public:

        virtual tvalue remove(tkey const &key)
        {

        }

    protected:
        void cleanup_node(node **node_address)
        {
            (*node_address)->~node();
            deallocate_with_guard(reinterpret_cast<void *>(*node_address));

            *node_address = nullptr;
        }

        virtual void after_remove(
                std::stack<node **> &path) const
        {

        }

    private:
        [[nodiscard]] memory *get_memory() const noexcept override
        {
            return this->_target_tree->_allocator;
        }

    public:

        explicit removing_template_method(
                b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : template_method_basics(target_tree)
        {

        }
    };
#pragma endregion
#pragma endregion

#pragma region associative container contract
    void insert(tkey const &key, tvalue &&value) override
    {
        this->_insertion->insert(key, std::move(value));
    }

    tvalue const &get(tkey const &key) override
    {
        return this->_finding->find(key);
    }

    tvalue remove(tkey const &key) override
    {
        return this->_removing->remove(key);
    }
#pragma endregion

#pragma region rule 5
protected:
    // constructor for use in rule 5
    b_tree(logger *logger, memory *allocator,
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
    // public constructor
    explicit b_tree(logger *logger = nullptr, memory *allocator = nullptr)
    : b_tree(logger,
            allocator,
            new insertion_template_method(this),
            new finding_template_method(this),
            new removing_template_method(this))
    {

    }

public:
    // copy constructor
    b_tree(b_tree<tkey, tvalue, tkey_comparer> const &obj)
    : b_tree(obj._logger, obj._allocator)
    {
        _root = copy(obj._root);
    }

    // move constructor
    b_tree(b_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    : b_tree(obj._insertion,
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

    // copy assignment (оператор присваивания)
    b_tree &operator=(b_tree<tkey, tvalue, tkey_comparer> const &obj)
    {
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
    b_tree &operator=(b_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    {
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
    ~b_tree()
    {
        delete _insertion;
        delete _finding;
        delete _removing;

        clearup(_root);
    }

private:

    void clearup(
            node *element)
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

    // TODO: think about usability in derived classes
    node *copy(
            node *from)
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        node *result = allocate_with_guard(sizeof(node));
        new (result) node(*from);

        result->left_subtree = copy(from->left_subtree);
        result->right_subtree = copy(from->right_subtree);

        return result;
    }
#pragma endregion


#pragma region logger and allocator contract
    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return _logger;
    }

    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return _allocator;
    }
#pragma endregion

};

#endif //B_TREE_H
