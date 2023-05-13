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

protected:
    class bt_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit bt_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

public:
    // todo: iterators

protected:
#pragma region template methods
    class template_method_basics:
            protected logger_holder
    {
        // TODO: split node, make it virtual
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    protected:

        b_tree<tkey, tvalue, tkey_comparer> *_target_tree;

        node *get_root_node() {
            return _target_tree->_root;
        }

        std::pair<std::stack<node **>, std::pair<node **, unsigned>> find_path(tkey const &key) const
        {
            // todo: should we break nodes here?
            std::stack<node **> path;

            if (_target_tree->_root == nullptr)
            {
                return { path, &_target_tree->_root };
            }

            node **iterator = &_target_tree->_root;
            tkey_comparer comparer;

            unsigned high = 0, low = 0, mid = 0;
            while (true)
            {
                low = 0;
                high = (*iterator)->_number_of_keys;

                while (low <= high) {
                    mid = (low + high) / 2;
                    auto comparison_result = comparer(key, (*iterator)->_keys[mid]);
                    if (comparison_result == 0) {
                        return std::pair<std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **>,
                                std::pair<typename b_tree<tkey, tvalue, tkey_comparer>::node**, unsigned>>(path, iterator, mid);
                    }

                    comparison_result < 0
                            ? (high = mid - 1)
                            : (low = mid + 1);
                }

                path.push(iterator);

                if (!((*iterator)->_is_leaf)) {
                    auto comparison_result = comparer(key, (*iterator)->_keys[mid]);
                    comparison_result > 0
                        ? ( iterator = &((*iterator)->_sub_nodes[mid + 1]) )
                        : ( iterator = &((*iterator)->_sub_nodes[mid]) );
                } else {
                    break;
                }
            }

            return std::pair<std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **>,
                    std::pair<typename b_tree<tkey, tvalue, tkey_comparer>::node**, unsigned>>(path, iterator, mid);;
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
        void insert(tkey const &key, tvalue &&value)
        {
            this->trace_with_guard("bs_tree::insertion_template_method::insert method started");

            // returned by find path is std::pair<std::stack<node **>, std::pair<node **, unsigned>>
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            node **target_ptr = path_and_target.second.first;
            unsigned target_index = path_and_target.second.second;

            if (tkey_comparer(key, (*target_ptr)->_keys[target_index]) == 0)
            {
                this->debug_with_guard("b_tree::insertion_template_method::insert passed key is not unique")
                    ->trace_with_guard("b_tree::insertion_template_method::insert method finished");
                throw bt_exception("b_tree::insertion_template_method::insert passed key is not unique");
            }

            this->trace_with_guard("b_tree::insertion_template_method::insert method finished");
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
        tvalue const &find(tkey const &key)
        {
            this->trace_with_guard("b_tree::finding_template_method::find method started");
            if (this->get_root_node() == nullptr) {
                this->information_with_guard("b_tree::finding_template_method::find the tree is empty")
                    ->trace_with_guard("b_tree::finding_template_method::find method finished");
            }

            // returned by find path is std::pair<std::stack<node **>, std::pair<node **, unsigned>>
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            node **target_ptr = path_and_target.second.first;
            unsigned index = path_and_target.second.second;

            if (tkey_comparer(key, (*target_ptr)->_keys[index]) != 0)
            {
                this->debug_with_guard("b_tree::finding_template_method::find no value with passed key in tree")
                    ->trace_with_guard("b_tree::finding_template_method::find method finished");
                throw bt_exception("b_tree::finding_template_method::find no value with passed key in tree");
            }

//            after_find_inner(path, target_ptr);

            this->trace_with_guard("b_tree::finding_template_method::find method finished");
            return (*target_ptr)->value;
        }

        virtual void after_find_inner(std::stack<node **> &path, node **target_ptr)
        {
            // TODO: nothing to do here in BT context...
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
            this->trace_with_guard("bs_tree::removing_template_method::remove method started");

            // returned by find path is std::pair<std::stack<node **>, std::pair<node **, unsigned>>
            auto path_and_target = this->find_path(key);
            auto path = path_and_target.first;
            node **target_ptr = path_and_target.second.first;
            unsigned target_index = path_and_target.second.second;

            if (tkey_comparer(key, (*target_ptr)->_keys[target_index]) != 0)
            {
                this->debug_with_guard("b_tree::removing_template_method::remove passed key is not found")
                    ->trace_with_guard("b_tree::removing_template_method::remove method finished");
                throw bt_exception("b_tree::removing_template_method::remove passed key is not found");
            }

            // 2 case: deleting from leaf and not-leaf
            // + 2 sub_cases: deleting from non-full node and full node

            this->trace_with_guard("b_tree::removing_template_method::remove method finished");
        }

    protected:
        void cleanup_node(node **node_address)
        {
            (*node_address)->~node();
            this->deallocate_with_guard(reinterpret_cast<void *>(*node_address));

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
