#ifndef B_TREE_H
#define B_TREE_H

#include <stack>
#include "../binary_tree/associative_container.h"
#include "../logger/logger.h"
#include "../logger/logger_holder.h"
#include "../allocator/memory_base_class.h"
#include "../allocator/memory_holder.h"
//todo: check tree parameter
template <
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class b_tree :
        public associative_container<tkey, tvalue>,
        protected memory_holder,
        protected logger_holder
{
public:
    // number of keys in nodes vary [t-1 ; 2t-1] excluding root where this number is (1 ; 2t-1]
    struct node
            : private memory_holder
        {
        friend class b_tree<tkey, tvalue, tkey_comparer>;

        b_tree<tkey, tvalue, tkey_comparer> * _target_tree;

        bool _is_leaf;
        unsigned _number_of_keys;
        tkey * _keys;
        tvalue * _values; // ??
        struct node ** _sub_nodes;

        node(b_tree<tkey, tvalue, tkey_comparer> * target_tree)
        : _target_tree(target_tree)
        {
            _is_leaf = true;
            _number_of_keys = 0;
            size_t max = 2 * _target_tree->_tree_parameter;
            _keys = reinterpret_cast<tkey *>(allocate_with_guard(sizeof(tkey) * (max - 1)));
            _values = reinterpret_cast<tvalue *>(allocate_with_guard(sizeof(tvalue) * (max - 1)));
            _sub_nodes = reinterpret_cast<node **>(allocate_with_guard(sizeof(node *) * (max)));

            (*_values) = nullptr;
        }

        virtual ~node()
        {
            deallocate_with_guard(_keys);
            deallocate_with_guard(_values);
            deallocate_with_guard(_sub_nodes);
        }

        memory *get_memory() const noexcept override
        {
            return _target_tree->get_memory();
        }
    };

protected:
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

public:
    unsigned get_tree_parameter() const
    {
        return _tree_parameter;
    }

protected:
    class insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit insert_exception(std::string const &message)
                : _message(message) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit find_exception(std::string const &message)
                : _message(message) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit remove_exception(std::string const &message)
                : _message(message) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class iterator_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit iterator_exception(std::string const &message)
                : _message(message) {

        }

        [[nodiscard]] char const *what() const noexcept override {
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

    public:
        // returns an index of element with such key or the closest one to key
        unsigned dichotomy_search(node * current_node, tkey const &key)
        {
            unsigned low = 0, mid = 0;
            unsigned high = current_node->_number_of_keys;

            while (low <= high) {
                mid = (low + high) / 2;
                auto comparison_result = comparer(key, current_node->_keys[mid]);
                if (comparison_result == 0) {
                    return mid;
                }

                comparison_result < 0
                ? (high = mid - 1)
                : (low = mid + 1);
            }
            return mid;
        }

    protected:
        b_tree<tkey, tvalue, tkey_comparer> *_target_tree;

        node *get_root_node() {
            return _target_tree->_root;
        }

        std::pair< std::stack<std::pair< node **, unsigned > >, std::pair< node **, unsigned > > find_path(tkey const &key) const
        {
            std::stack<std::pair< node **, unsigned >> path;

            if (_target_tree->_root == nullptr)
            {
                return std::pair<std::stack<std::pair<node **, unsigned >>,
                        std::pair<typename b_tree<tkey, tvalue, tkey_comparer>::node**, unsigned>>( path, &_target_tree->_root, 0 );
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
                        return std::pair<std::stack<std::pair<node **, unsigned >>,
                                std::pair<node**, unsigned>>(path, iterator, mid);
                    }

                    comparison_result < 0
                            ? (high = mid - 1)
                            : (low = mid + 1);
                }

                path.push(std::pair< node **, unsigned >(iterator, mid));

                if (!((*iterator)->_is_leaf)) {
                    auto comparison_result = comparer(key, (*iterator)->_keys[mid]);
                    comparison_result > 0
                        ? ( iterator = &((*iterator)->_sub_nodes[mid + 1]) )
                        : ( iterator = &((*iterator)->_sub_nodes[mid]) );
                } else {
                    break;
                }
            }

            return std::pair<std::stack<std::pair<node **, unsigned >>,
                    std::pair<node**, unsigned>>(path, iterator, mid);;
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
    protected:
        // returns an index of inserted element
        virtual unsigned insert_key_in_nonfull_node(node * current_node, tkey const &key, tvalue &&value)
        {
            this->trace_with_guard("b_tree::insertion_template_method::insert_key_in_nonfull_node method started");

            if (current_node->_number_of_keys == 0) {
                // insertion in root
                current_node->_keys[0] = key;
                current_node->_values[0] = std::move(value);
                current_node->_number_of_keys = 1;
                current_node->_sub_nodes[current_node->_number_of_keys] = nullptr;

                this->trace_with_guard("b_tree::insertion_template_method::insert_key_in_nonfull_node method started");
                return 0;
            }

            unsigned closest_index = this->dichotomy_search(current_node, key);
            auto comparison_result = tkey_comparer(key, current_node->_keys[closest_index]);

            if (comparison_result == 0) {
                throw insert_exception("b_tree::insertion_template_method::insert_key_in_nonfull_node key is not unique");
            }

            unsigned number_of_keys_in_node = current_node->_number_of_keys;
            unsigned position_of_key_to_insert = (comparison_result > 0 ? closest_index + 1 : closest_index);
            unsigned shift = position_of_key_to_insert + 1;
            unsigned count_of_keys_to_shift = number_of_keys_in_node - position_of_key_to_insert;
            // element_to_insert is not the last element in key/value array
            if (count_of_keys_to_shift) {
                memmove((current_node->_keys + shift), (current_node->_keys + position_of_key_to_insert), sizeof(tkey) * count_of_keys_to_shift);
                memmove((current_node->_values + shift), (current_node->_values + position_of_key_to_insert), sizeof(tvalue) * count_of_keys_to_shift);
                memmove((current_node->_sub_nodes + position_of_key_to_insert + 2), (current_node->_sub_nodes + position_of_key_to_insert + 1), sizeof(node *) * count_of_keys_to_shift);
                current_node->_sub_nodes[position_of_key_to_insert + 1] = nullptr;
            }
            current_node->_keys[position_of_key_to_insert] = key;
            current_node->_values[position_of_key_to_insert] = std::move(value);
            current_node->_number_of_keys++;

            this->trace_with_guard("b_tree::insertion_template_method::insert_key_in_nonfull_node method started");
            return position_of_key_to_insert;
        }

    public:
        // returns a node where key value may, possibly, be inserted
        virtual node ** split(std::stack<std::pair< node **, unsigned >> &path, node ** current_node, tkey const &key)
        {
            this->trace_with_guard("b_tree::insertion_template_method::split method started");

            node ** split_element_left_subtree, **split_element_right_subtree;
            unsigned mid_index = (*current_node)->_number_of_keys / 2;
            auto comparison_result = tkey_comparer(key, (*current_node)->_keys[mid_index]);

            if (path.empty()) {
                // splitting root
                path.push(std::pair<node **, unsigned >(current_node, 0));

                split_element_left_subtree = &((*current_node)->_sub_nodes[0]);
                split_element_right_subtree = &((*current_node)->_sub_nodes[1]);
                initialize_memory_with_node((*split_element_left_subtree));
                memmove((*split_element_left_subtree)->_keys, (*current_node)->_keys, sizeof(tkey) * mid_index);
                memmove((*split_element_left_subtree)->_values, (*current_node)->_values, sizeof(tvalue) * mid_index);
                memmove((*split_element_left_subtree)->_sub_nodes, (*current_node)->_sub_nodes, sizeof(node *) * (mid_index + 1));
                (*split_element_left_subtree)->_number_of_keys = mid_index;

                initialize_memory_with_node((*split_element_right_subtree));
                memmove((*split_element_right_subtree)->_keys, ((*current_node)->_keys + mid_index + 1), sizeof(tkey) * mid_index);
                memmove((*split_element_right_subtree)->_values, ((*current_node)->_values + mid_index + 1), sizeof(tvalue) * mid_index);
                memmove((*split_element_right_subtree)->_sub_nodes, ((*current_node)->_sub_nodes + mid_index + 1), sizeof(node *) * (mid_index + 1));
                (*split_element_right_subtree)->_number_of_keys = mid_index;

                (*current_node)->_number_of_keys = 1;
                (*current_node)->_keys[0] = (*current_node)->_keys[mid_index];
                (*current_node)->_values[0] = std::move((*current_node)->_values[mid_index]);

            } else {
                // splitting not root
                // current node will be a left subtree to mid element
                auto tmp = path.top();
                node **parent_node = tmp.first;

                // todo: move key as well? so current node wouldn't have a ghost key
                // parent element is 100% not full, so we can use insert_key_in_nonfull_node here
                unsigned mid_element_position_in_parent_node = insert_key_in_nonfull_node((*parent_node), ((*current_node)->_keys[mid_index]), std::move((*current_node)->_values[mid_index]));

                split_element_left_subtree = current_node;
                (*split_element_left_subtree)->_number_of_keys = mid_index;
                split_element_right_subtree = &((*parent_node)->_sub_nodes[mid_element_position_in_parent_node + 1]);

                initialize_memory_with_node((*split_element_right_subtree));
                memmove(((*split_element_right_subtree)->_keys), ((*current_node)->_keys + mid_index + 1), sizeof(tvalue) * mid_index);
                memmove(((*split_element_right_subtree)->_values), ((*current_node)->_values + mid_index + 1), sizeof(tvalue) * mid_index);
                memmove(((*split_element_right_subtree)->_sub_nodes), ((*current_node)->_sub_nodes + mid_index + 1), sizeof(node *) + (mid_index + 1))
                (*split_element_right_subtree)->_number_of_keys = mid_index;
            }
            this->trace_with_guard("b_tree::insertion_template_method::split method finished");
            return (comparison_result < 0 ? split_element_left_subtree : split_element_right_subtree);
        }

        void insert(tkey const &key, tvalue &&value)
        {
            this->trace_with_guard("bs_tree::insertion_template_method::insert method started");
            node ** current_node = &(this->_target_tree->_root);

            if ((*current_node) == nullptr) {
                // tree is empty
                *current_node = reinterpret_cast<node *>(allocate_with_guard(get_node_size()));
                initialize_memory_with_node(*current_node);
                insert_key_in_nonfull_node(*current_node, key, std::move(value));
                return;
            }

            bool current_key_is_full = false;
            std::stack<std::pair< node **, unsigned >> path;
            unsigned max_number_of_keys_in_node = this->_target_tree->_tree_parameter * 2 - 1;
            unsigned closest_index = 0;

            while (((*current_node) != nullptr) || (current_key_is_full = ((*current_node)->_number_of_keys == max_number_of_keys_in_node))) {
                // split and go down to leaf
                if (current_key_is_full) {
                    current_node = split(path, current_node, key);
                }

                closest_index = this->dichotomy_search((*current_node), key);

                // if under index returned is the same key, throw, key not unique
                auto comparison_result = tkey_comparer(key, (*current_node)->_keys[closest_index]);
                if (comparison_result == 0) {
                    throw insert_exception("b_tree::insertion_template_method::insert::insert_key_in_nonfull_node key is not unique");
                }

                if (!((*current_node)->_is_leaf)) {
                    // push in path, go down
                    path.push(std::pair< node**, unsigned >(current_node, closest_index));
                    current_node = (comparison_result < 0
                            ? &((*current_node)->_sub_nodes[closest_index])
                            : &((*current_node)->_sub_nodes[closest_index + 1]));
                } else {
                    // insert an element
                    insert_key_in_nonfull_node((*current_node), key, std::move(value));
                }
            }

            this->trace_with_guard("b_tree::insertion_template_method::insert method finished");
        }

        [[nodiscard]] virtual size_t get_node_size() const
        {
            return sizeof(node);
        }

        virtual void initialize_memory_with_node(node *target_ptr) const
        {
            new(target_ptr) node(this->_target_tree);
//            new(target_ptr) node();
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
                throw find_exception("b_tree::finding_template_method::find no value with passed key in tree");
            }

            this->trace_with_guard("b_tree::finding_template_method::find method finished");
            return (*target_ptr)->value;
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
                throw remove_exception("b_tree::removing_template_method::remove passed key is not found");
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

    void clearup(node *element)
    {
        if (element == nullptr)
        {
            return;
        }

        if (!(element->_is_leaf)) {
            unsigned i, count_of_subtrees = element->_number_of_keys + 1;
            for (i = 0; i < count_of_subtrees; i++) {
                clearup(element->_sub_nodes[i]);
            }
        }

        element->~node();
        deallocate_with_guard(element);
    }

    node *copy(node *from)
    {
        if (from == nullptr)
        {
            return nullptr;
        }

        node *result = allocate_with_guard(sizeof(node));
        new (result) node(*from);

        memmove(result->_keys, from->_keys, sizeof(tkey) * from->_number_of_keys);

        if (!(from->_is_leaf)) {
            unsigned i, count_of_subtrees = from->_number_of_keys + 1;
            for (i = 0; i < count_of_subtrees; i++) {
                result->_sub_nodes[i] = copy(from->_sub_nodes[i]);
            }
        }

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
