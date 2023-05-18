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

#pragma region exceptions
public:
    class b_tree_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit b_tree_exception(std::string const &message)
                : _message(message) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

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
#pragma endregion

public:
#pragma region iterator
    class iterator final
    {
        // iterates: first -- node, then its sub-nodes
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    private:
        node *_current_node;
        std::stack<std::pair<node *, unsigned> > _path; // parent node and index of sub-node
        unsigned _parent_element_index;

    public:
        explicit iterator(node *current_node)
        {
            this->_current_node = current_node;
            _parent_element_index = 0;
        }

        bool operator==(iterator const &other) const
        {
            return (_current_node == other._current_node);
        }

        bool operator!=(iterator const &other) const
        {
            return (_current_node != other._current_node);
        }

        iterator& operator++()
        {
            if (_current_node == nullptr) {
                throw iterator_exception("b_tree::iterator iterator is out of range");
            }

            unsigned index_of_last_sub_node_in_parent_node = 0;

            if (_current_node->_is_leaf) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    return *this;
                }

                while (!(_path.empty())) {
                    auto parent_and_index_of_this_sub_node = _path.top();
                    node * parent_node = parent_and_index_of_this_sub_node.first;
                    unsigned index_of_this_sub_node = parent_and_index_of_this_sub_node.second;
                    _path.pop();

                    index_of_last_sub_node_in_parent_node = parent_node->_number_of_keys;
                    if (index_of_this_sub_node != index_of_last_sub_node_in_parent_node) {
                        // go to the next sub-node, if current is not the last
                        index_of_this_sub_node++;
                        _path.push(std::pair<node *, unsigned >(parent_node, index_of_this_sub_node));
                        _current_node = parent_node->_sub_nodes[index_of_this_sub_node];
                        return *this;
                    }
                    // current sub-node is the last, thus go to the grandparent
                }

                if (_path.empty()) {
                    _current_node = nullptr;
//                    return *this;
                }
            }
            else {
                // current node is an inner node, so we need to go to its first sub-node
                _path.push(std::pair<node *, unsigned >(_current_node, 0));
                _current_node = _current_node->_sub_nodes[0];
            }

            return *this;
        }

        iterator operator++(int not_used)
        {
            auto result = *this;
            ++*this;
            return *this;
        }

        std::tuple<unsigned int, const tkey*, const tvalue*, unsigned int > operator*() const
        {
            return std::tuple<unsigned int, const tkey*, const tvalue*, unsigned int >(_path.size(), _current_node->_keys, _current_node->_values, _current_node->_number_of_keys);
        }

    };

public:
    iterator begin_iter() const noexcept
    {
        return b_tree<tkey, tvalue, tkey_comparer>::iterator(_root);
    }

    iterator end_iter() const noexcept
    {
        return b_tree<tkey, tvalue, tkey_comparer>::iterator(nullptr);
    }

#pragma endregion

protected:
#pragma region template methods
    class template_method_basics:
            protected logger_holder,
            protected memory_holder
    {
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    public:
        // returns an index of element with such key or the closest one to key
        unsigned dichotomy_search(node * current_node, tkey const &key)
        {
            int low = 0, mid = 0;
            int high = current_node->_number_of_keys;
            tkey_comparer comparer;

            // low <= high gives wrong closest index (?)
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

            if (mid == current_node->_number_of_keys) {
                mid--;
            }

            return mid;
        }

    protected:
        b_tree<tkey, tvalue, tkey_comparer> *_target_tree;

        void cleanup_node(node **node_address)
        {
            (*node_address)->~node();
            this->deallocate_with_guard(reinterpret_cast<void *>(*node_address));

            *node_address = nullptr;
        }

        node *get_root_node() {
            return _target_tree->_root;
        }

        // returns an index of inserted element
        virtual unsigned insert_key_in_nonfull_node(node * current_node, tkey const &key, tvalue &value)
        {
            this->trace_with_guard("b_tree::template_method_basics::insert_key_in_nonfull_node method started");

            if (current_node->_number_of_keys == 0) {
                // insertion in root
                current_node->_keys[0] = key;
                current_node->_values[0] = value;
//                current_node->_values[0] = std::move(value);
                current_node->_number_of_keys = 1;

                this->trace_with_guard("b_tree::template_method_basics::insert_key_in_nonfull_node method finished");
                return 0;
            }

            unsigned closest_index = this->dichotomy_search(current_node, key);

            if (closest_index == current_node->_number_of_keys) {
                closest_index--;
            }

            tkey_comparer comparer;

            auto comparison_result = comparer(key, current_node->_keys[closest_index]);

            if (comparison_result == 0) {
                this->warning_with_guard("b_tree::template_method_basics::insert_key_in_nonfull_node key is not unique")
                    ->trace_with_guard("b_tree::template_method_basics::insert_key_in_nonfull_node method finished");
                throw insert_exception("b_tree::template_method_basics::insert_key_in_nonfull_node key is not unique");
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
//                current_node->_sub_nodes[position_of_key_to_insert + 1] = nullptr;
            }
            current_node->_keys[position_of_key_to_insert] = key;
            current_node->_values[position_of_key_to_insert] = value;
//            current_node->_values[position_of_key_to_insert] = std::move(value);
            current_node->_number_of_keys++;

            this->trace_with_guard("b_tree::template_method_basics::insert_key_in_nonfull_node method started");
            return position_of_key_to_insert;
        }

        std::pair< std::stack<std::pair< node **, unsigned > >, std::pair< node **, unsigned > > find_path(tkey const &key) const
        {
            std::stack<std::pair< node **, unsigned >> path;

            if (_target_tree->_root == nullptr)
            {
                std::pair<node**, unsigned> pair (&_target_tree->_root, 0);
                return std::pair<std::stack<std::pair<node **, unsigned >>,
                        std::pair<node**, unsigned>>( path, pair );
            }

            node **iterator = &_target_tree->_root;
            tkey_comparer comparer;

            int high = 0, low = 0, mid = 0;
            while (true)
            {
                low = 0;
                high = (*iterator)->_number_of_keys;

                while (low <= high) {
                    mid = (low + high) / 2;
                    auto comparison_result = comparer(key, (*iterator)->_keys[mid]);

                    if (mid != (*iterator)->_number_of_keys) {
                        if (comparison_result == 0) {
                            this->debug_with_guard("b_tree::template_methods::basics::find_path current node count: " +
                                                   std::to_string((*iterator)->_number_of_keys) + " index: " +
                                                   std::to_string(mid));
                            std::pair<node**, unsigned> pair (iterator, mid);
                            return std::pair<std::stack<std::pair<node **, unsigned >>,
                                    std::pair<node**, unsigned>>(path, pair);
                        }
                    }

                    comparison_result <= 0
                            ? (high = mid - 1)
                            : (low = mid + 1);
                }

                if (mid == (*iterator)->_number_of_keys) {
                    mid--;
                }
//                path.push(std::pair< node **, unsigned >(iterator, mid));
                this->debug_with_guard("b_tree::template_methods::basics::find_path current node count:" +
                                       std::to_string((*iterator)->_number_of_keys) + " index: " +
                                       std::to_string(mid));
                if (!((*iterator)->_is_leaf)) {
                    path.push(std::pair< node **, unsigned >(iterator, mid));

                    auto comparison_result = comparer(key, (*iterator)->_keys[mid]);
                    comparison_result > 0
                        ? ( iterator = &((*iterator)->_sub_nodes[mid + 1]) )
                        : ( iterator = &((*iterator)->_sub_nodes[mid]) );
                } else {
                    break;
                }
            }

            std::pair<node**, unsigned> pair (iterator, mid);
            return std::pair<std::stack<std::pair<node **, unsigned >>,
                    std::pair<node**, unsigned>>(path, pair);
        }

    private:
        logger *get_logger() const noexcept override
        {
            return _target_tree->get_logger();
        }

    private:
        [[nodiscard]] memory *get_memory() const noexcept override
        {
            return _target_tree->get_memory();
        }

    public:
        explicit template_method_basics(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : _target_tree(target_tree)
        {
        }
    };

#pragma region insertion template method
    class insertion_template_method :
            public template_method_basics
    {
    protected:

    public:
        void split_root(node ** root_node, unsigned mid_index, node ** split_element_left_subtree, node ** split_element_right_subtree)
        {
            node * new_root_node = new node(this->_target_tree);


            // move t-1 elements in left and right subtrees from root node

            // move medium element to the beginning of the array

        }

        // returns a node in which key-value pair may, possibly, be inserted
        virtual node ** split(std::stack<std::pair< node **, unsigned >> &path, node ** current_node, tkey const &key)
        {
            this->trace_with_guard("b_tree::insertion_template_method::split method started");

            node ** split_element_left_subtree, **split_element_right_subtree;
            unsigned mid_index = (*current_node)->_number_of_keys / 2;
            tkey_comparer comparer;
            auto comparison_result = comparer(key, (*current_node)->_keys[mid_index]);

            if (path.empty()) {
                // splitting root
                path.push(std::pair<node **, unsigned >(current_node, 0));
                node** root_node = current_node;
                node * new_root_node = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));
                initialize_memory_with_node(new_root_node);

                new_root_node->_is_leaf = false;
                new_root_node->_number_of_keys = 1;
                new_root_node->_keys[0] = (*current_node)->_keys[mid_index];
                new_root_node->_values[0] = (*current_node)->_values[mid_index];
                // new_root_node->_values[0] = std::move((*current_node)->_values[mid_index]);

                split_element_left_subtree = &(new_root_node->_sub_nodes[0]);
                split_element_right_subtree = &(new_root_node->_sub_nodes[1]);

                (*split_element_left_subtree) = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));
                (*split_element_right_subtree) = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));

                initialize_memory_with_node((*split_element_left_subtree));
                initialize_memory_with_node((*split_element_right_subtree));

                if (!((*root_node)->_is_leaf)) {
                    (*split_element_left_subtree)->_is_leaf = false;
                    (*split_element_right_subtree)->_is_leaf = false;
                }

                memmove((*split_element_left_subtree)->_keys, (*current_node)->_keys, sizeof(tkey) * mid_index);
                memmove((*split_element_left_subtree)->_values, (*current_node)->_values, sizeof(tvalue) * mid_index);
                memmove((*split_element_left_subtree)->_sub_nodes, (*current_node)->_sub_nodes, sizeof(node *) * (mid_index + 1));
                (*split_element_left_subtree)->_number_of_keys = mid_index;

                memmove((*split_element_right_subtree)->_keys, ((*current_node)->_keys + mid_index + 1), sizeof(tkey) * mid_index);
                memmove((*split_element_right_subtree)->_values, ((*current_node)->_values + mid_index + 1), sizeof(tvalue) * mid_index);
                memmove((*split_element_right_subtree)->_sub_nodes, ((*current_node)->_sub_nodes + mid_index + 1), sizeof(node *) * (mid_index + 1));
                (*split_element_right_subtree)->_number_of_keys = mid_index;

                this->cleanup_node(root_node);
                (*root_node) = new_root_node;
            } else {
                // splitting not root
                // current node will be a left subtree to mid element
                auto tmp = path.top();
                node **parent_node = tmp.first;

                unsigned mid_element_position_in_parent_node = this->insert_key_in_nonfull_node((*parent_node), ((*current_node)->_keys[mid_index]), (*current_node)->_values[mid_index]);

                split_element_left_subtree = current_node;
                (*split_element_left_subtree)->_number_of_keys = mid_index;
                split_element_right_subtree = &((*parent_node)->_sub_nodes[mid_element_position_in_parent_node + 1]);
                (*split_element_right_subtree) = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));
                initialize_memory_with_node((*split_element_right_subtree));

                memmove(((*split_element_right_subtree)->_keys), ((*current_node)->_keys + mid_index + 1), sizeof(tkey) * mid_index);
                memmove(((*split_element_right_subtree)->_values), ((*current_node)->_values + mid_index + 1), sizeof(tvalue) * mid_index);
                memmove(((*split_element_right_subtree)->_sub_nodes), ((*current_node)->_sub_nodes + mid_index + 1), sizeof(node *) + (mid_index + 1));
                (*split_element_right_subtree)->_number_of_keys = mid_index;
            }
            this->trace_with_guard("b_tree::insertion_template_method::split method finished");
            return (comparison_result < 0 ? split_element_left_subtree : split_element_right_subtree);
        }

        void insert(tkey const &key, tvalue &value)
        {
            this->trace_with_guard("bs_tree::insertion_template_method::insert method started");
            node ** current_node = &(this->_target_tree->_root);

            if ((*current_node) == nullptr) {
                // tree is empty
                *current_node = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));
                initialize_memory_with_node(*current_node);
                this->insert_key_in_nonfull_node(*current_node, key, value);
//                this->insert_key_in_nonfull_node(*current_node, key, std::move(value));

                return;
            }

            bool current_key_is_full = false;
            std::stack<std::pair< node **, unsigned >> path;
            unsigned max_number_of_keys_in_node = this->_target_tree->_tree_parameter * 2 - 1;
            unsigned closest_index = 0;
            tkey_comparer comparer;

            while (((*current_node) != nullptr) || (*current_node)->_number_of_keys == max_number_of_keys_in_node) {
                // split and go down to leaf
                current_key_is_full = (*current_node)->_number_of_keys == max_number_of_keys_in_node;
                if (current_key_is_full) {
                    current_node = split(path, current_node, key);
                }

                closest_index = this->dichotomy_search((*current_node), key);

                // if under index returned is the same key, throw, key not unique
                auto comparison_result = comparer(key, (*current_node)->_keys[closest_index]);
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
                    this->insert_key_in_nonfull_node((*current_node), key, value);
                    break;
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
//            target_ptr = new node(this->_target_tree);
            new(target_ptr) node(this->_target_tree);
//            new(target_ptr) node();
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
            tkey_comparer comparer;

            if (comparer(key, (*target_ptr)->_keys[index]) != 0)
            {
                this->debug_with_guard("b_tree::finding_template_method::find no value with passed key in tree")
                    ->trace_with_guard("b_tree::finding_template_method::find method finished");
                throw find_exception("b_tree::finding_template_method::find no value with passed key in tree");
            }

            this->trace_with_guard("b_tree::finding_template_method::find method finished");
            return (*target_ptr)->_values[index];
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
            public template_method_basics
    {
    public:

        // function does not deletes node. It simply moves arrays. Moves subnodes from [index_of_deleted + 1]
        void delete_key_value_from_node(node * current_node, unsigned index_of_deleted, bool left_subtree_to_move)
        {
            unsigned count_of_elements_to_shift = current_node->_number_of_keys - index_of_deleted - 1;
            unsigned from = index_of_deleted + 1;
            if (count_of_elements_to_shift != 0) {
                // current_node is not the last element in the array
                memmove(current_node->_keys + index_of_deleted, current_node->_keys + from, sizeof(tkey) * count_of_elements_to_shift);
                memmove(current_node->_values + index_of_deleted, current_node->_values + from, sizeof(tvalue) * count_of_elements_to_shift);
                if (left_subtree_to_move) {
                    memmove(current_node->_sub_nodes + index_of_deleted, current_node->_sub_nodes + from, sizeof(node *) * count_of_elements_to_shift);
                } else {
                    memmove(current_node->_sub_nodes + index_of_deleted + 1, current_node->_sub_nodes + from + 1, sizeof(node *) * count_of_elements_to_shift);
                }
            }
            current_node->_number_of_keys--;
        }

        std::pair<node **, node**> find_brother_nodes(node ** parent, unsigned parent_index, node ** current_node)
        {
            node ** brother_left = nullptr, **brother_right = nullptr;
            unsigned brother_left_index = 0, brother_right_index = 0;
            // find brother nodes
            if ((*parent)->_sub_nodes[parent_index] == (*current_node)) {
                // current node is left subtree of its parent
                brother_left_index = parent_index - 1; brother_right_index = parent_index + 1;
                // checkup
                if ((*parent)->_sub_nodes[0] != (*current_node)) {
                    brother_left = &((*parent)->_sub_nodes[brother_left_index]);
                }
            }
            else {
                // current node is right subtree of its parent
                brother_left_index = parent_index; brother_right_index = parent_index + 2;
                brother_left = &((*parent)->_sub_nodes[brother_left_index]);
            }

            // checkup
            if ((*parent)->_sub_nodes[(*parent)->_number_of_keys] != (*current_node)) {
                brother_right = &((*parent)->_sub_nodes[brother_right_index]);
            }

            return std::pair<node **, node**>(brother_left, brother_right);
        }

        std::pair<node **, node**> find_enough_and_not_enough_keys_brother(node ** brother_left,
                                                                           node ** brother_right,
                                                                           node ** parent,
                                                                           unsigned* div_element_index,
                                                                           unsigned* common_parent_index,
                                                                           unsigned parent_index)
        {
            unsigned min_count_of_keys_in_node = this->_target_tree->_tree_parameter - 1;
            node **brother_with_enough_keys = nullptr, **brother_to_merge_with = nullptr;
            if (brother_left != nullptr) {
                brother_to_merge_with = brother_left;
                if ((*brother_left)->_number_of_keys > min_count_of_keys_in_node) {
                    // the last key in brother_with_enough_keys will be the closest one in brother_with_enough_keys to common parent key
                    brother_with_enough_keys = brother_left;
                    (*div_element_index) = (*brother_with_enough_keys)->_number_of_keys - 1;
                }
                (*common_parent_index) = ((*parent)->_sub_nodes[parent_index] == (*brother_left)
                        ? parent_index : parent_index - 1);
            }
            if (brother_right != nullptr && brother_with_enough_keys == nullptr) {
                brother_to_merge_with = brother_right;
                if ((*brother_right)->_number_of_keys > min_count_of_keys_in_node) {
                    // the last key in brother_with_enough_keys will be the closest one in brother_with_enough_keys to common parent key
                    brother_with_enough_keys = brother_right;
                    (*div_element_index) = 0;
                }
                (*common_parent_index) = ((*parent)->_sub_nodes[parent_index] == (*brother_right)
                                          ? parent_index - 1 : parent_index );
            }
            return std::pair<node **, node**>(brother_with_enough_keys, brother_to_merge_with);
        }


        void delete_with_brother_with_enough_keys(node **brother,
                                                  node ** current_node,
                                                  unsigned index_of_deleted,
                                                  node ** parent,
                                                  unsigned common_parent_index,
                                                  unsigned div_element_index)
        {
            // found brother_with_enough_keys node with count of keys > t-1
            // delete key-value from current node, move array
            delete_key_value_from_node((*current_node), index_of_deleted, false);
            // insert common parent in current node
            this->insert_key_in_nonfull_node((*current_node), (*parent)->_keys[common_parent_index], (*parent)->_values[common_parent_index]);

            // insert in common parent node element, delete it from brother_with_enough_keys (brother_with_enough_keys is also list with count of elements > t-1, thus we can simply move arrays of keys and values to "delete" div key-value pair)
            (*parent)->_keys[common_parent_index] = (*brother)->_keys[div_element_index];
            (*parent)->_values[common_parent_index] = (*brother)->_values[div_element_index];
            delete_key_value_from_node((*brother), div_element_index, false);
        }

        // merges brother with current node, excluding deleted node. If brother is left to current node, do not pass to_merge_with_count_of_keys
        void merge_nodes(node * brother,
                         node ** current_node,
                         unsigned index_of_deleted,
                         const unsigned to_merge_with_count_of_keys = 0)
        {
            // merge current node with its brother
            unsigned count_of_elements_before_key_to_delete = index_of_deleted, count_of_elements_after_key_to_delete = (*current_node)->_number_of_keys - index_of_deleted - 1;
            // merging to brother part of keys before an element to delete
            if (index_of_deleted != 0) {
                // there are elements before deleted
                memmove((brother->_keys + to_merge_with_count_of_keys), (*current_node)->_keys, sizeof(tkey) * count_of_elements_before_key_to_delete);

                memmove((brother->_values + to_merge_with_count_of_keys), (*current_node)->_values, sizeof(tvalue) * count_of_elements_after_key_to_delete);

                brother->_number_of_keys += count_of_elements_before_key_to_delete;
            }

            if (index_of_deleted != (*current_node)->_number_of_keys - 1) {
                // there are element after deleted node
                memmove((brother->_keys + to_merge_with_count_of_keys), (*current_node)->_keys + count_of_elements_before_key_to_delete + 1, sizeof(tkey) * count_of_elements_after_key_to_delete);

                memmove((brother->_values + to_merge_with_count_of_keys), (*current_node)->_values + count_of_elements_before_key_to_delete + 1, sizeof(tvalue) * count_of_elements_after_key_to_delete);
                brother->_number_of_keys += count_of_elements_after_key_to_delete;
            }

            this->cleanup_node(current_node);

            /*
            // merge current node with its brother_with_not_enough_keys (or perform smart merging without adding to brother a key to delete)
            unsigned count_of_elements_before_key_to_delete = index_of_deleted, count_of_elements_after_key_to_delete = (*current_node)->_number_of_keys - index_of_deleted - 1;
            // merging to brother part of keys before an element to delete
            if (index_of_deleted != 0) {
                memmove(((*brother)->_keys + (*to_merge_with_count_of_keys)), (*current_node)->_keys, sizeof(tkey) * count_of_elements_before_key_to_delete);
                memmove(((*brother)->_values + (*to_merge_with_count_of_keys)), (*current_node)->_values, sizeof(tvalue) * count_of_elements_before_key_to_delete);
                (*brother)->_number_of_keys += count_of_elements_before_key_to_delete;
            }
            if (index_of_deleted != (*current_node)->_number_of_keys - 1) {
                memmove(((*brother)->_keys + (*to_merge_with_count_of_keys)), ((*current_node)->_keys + count_of_elements_before_key_to_delete + 1), sizeof(tkey) * count_of_elements_after_key_to_delete);
                memmove(((*brother)->_values + (*to_merge_with_count_of_keys)), ((*current_node)->_values + count_of_elements_before_key_to_delete + 1), sizeof(tkey) * count_of_elements_after_key_to_delete);
                (*brother)->_number_of_keys += count_of_elements_after_key_to_delete;
            }
            // delete current node
            this->cleanup_node(current_node);
             */
        }

        virtual void remove_inner(std::stack<std::pair< node **, unsigned >> &path, node ** current_node, unsigned index_of_deleted)
        {
            this->trace_with_guard("b_tree::removing_template_method::remove_inner method started")
                ->debug_with_guard("deleting from node with key count: " + std::to_string((*current_node)->_number_of_keys) + " index: " +
                                           std::to_string(index_of_deleted));
            unsigned min_count_of_keys_in_node = this->_target_tree->_tree_parameter - 1;
            if ((*current_node)->_is_leaf) {
                this->debug_with_guard("b_tree::removing_template_method::remove_inner removing from a leaf");
                if (path.empty() || (*current_node)->_number_of_keys > min_count_of_keys_in_node) {
                    // there are enough items for correct structure of b tree or this is root
                    delete_key_value_from_node((*current_node), index_of_deleted, false);

                    if ((*current_node)->_number_of_keys == 0) {
                        this->cleanup_node(current_node);
                    }
                    return;
                }

                std::pair< node **, unsigned > parent_index_pair = path.top();
                unsigned parent_index = parent_index_pair.second, common_parent_index = 0, div_element_index = 0;
                node ** parent = parent_index_pair.first;

                auto brothers_pair = find_brother_nodes(parent, parent_index, current_node);
                node **brother_left = brothers_pair.first, **brother_right = brothers_pair.second;
                auto brothers = find_enough_and_not_enough_keys_brother(brother_left, brother_right, parent, &div_element_index, &common_parent_index, parent_index);
                node **brother_with_enough_keys = brothers.first, **brother_to_merge_with = brothers.second;

                if (brother_with_enough_keys != nullptr) {
                    delete_with_brother_with_enough_keys(brother_with_enough_keys, current_node, index_of_deleted, parent, common_parent_index, div_element_index);
                }
                else {
                    unsigned * to_merge_with_count_of_keys = &((*brother_to_merge_with)->_number_of_keys);

                    // merging with left brother, adding elements to ends of arrays
                    if (brother_to_merge_with == brother_left) {
                        (*brother_to_merge_with)->_keys[(*to_merge_with_count_of_keys)] = (*parent)->_keys[common_parent_index];
                        (*brother_to_merge_with)->_values[(*to_merge_with_count_of_keys)] = (*parent)->_values[common_parent_index];
                        (*brother_to_merge_with)->_number_of_keys++;

                        merge_nodes(*brother_to_merge_with, current_node, index_of_deleted, *to_merge_with_count_of_keys);

                        // delete common parent from parent node
                        delete_key_value_from_node((*parent), parent_index, false);
                    }
                    // merge with right brother: move elements in right brother, add elements to the beginnings of the arrays
                    else {
                        memmove(((*brother_to_merge_with)->_keys + (*current_node)->_number_of_keys), (*brother_to_merge_with)->_keys, sizeof(tkey) * (*to_merge_with_count_of_keys));
                        memmove(((*brother_to_merge_with)->_values + (*current_node)->_number_of_keys), (*brother_to_merge_with)->_values, sizeof(tvalue) * (*to_merge_with_count_of_keys));

                        (*brother_to_merge_with)->_keys[(*current_node)->_number_of_keys - 1] = (*parent)->_keys[common_parent_index];
                        (*brother_to_merge_with)->_values[(*current_node)->_number_of_keys - 1] = (*parent)->_values[common_parent_index];
                        (*brother_to_merge_with)->_number_of_keys++;

                        merge_nodes(*brother_to_merge_with, current_node, index_of_deleted);

                        // delete common parent from parent node
                        delete_key_value_from_node((*parent), parent_index, true);
                    }

                    // todo: prove links are correct if parent node has more than 1 element before merging
                    if ((*parent)->_number_of_keys == 0) {
                        // parent is root. brother to merge with will be a new root
                        (*parent) = (*brother_to_merge_with);
                    }
                }
            }
            else
            // deleting an element not from leaf, but from inner node
            {
                this->debug_with_guard("b_tree::removing_template_method::remove_inner removing from an inner node");
                // find sub nodes
                node **left_sub_node = &((*current_node)->_sub_nodes[index_of_deleted]), **right_sub_node = &((*current_node)->_sub_nodes[index_of_deleted + 1]);
                // find if there is one that has > t-1 elements
                node ** suitable_node = nullptr;
                if ((*left_sub_node)->_number_of_keys > min_count_of_keys_in_node) {
                    suitable_node = left_sub_node;
                } else if ((*right_sub_node)->_number_of_keys > min_count_of_keys_in_node) {
                    suitable_node = right_sub_node;
                }

                if (suitable_node != nullptr) {
                    this->debug_with_guard("b_tree::removing_template_method::remove_inner there is a sub-node with count: " +
                                                   std::to_string((*suitable_node)->_number_of_keys) + " > " +
                                                   std::to_string(min_count_of_keys_in_node));
                    // if at least one of sub nodes has a count of elements > t-1

                    // find the closest key to d in sub node (the last in left subtree and the first in right one)
                    unsigned index_closest_to_key_to_d = (suitable_node == left_sub_node
                            ? ((*suitable_node)->_number_of_keys - 1)
                            : 0);

                    // (swap) replace d and closest key
                    tkey tmp_k = (*current_node)->_keys[index_of_deleted];
                    (*current_node)->_keys[index_of_deleted] = (*suitable_node)->_keys[index_closest_to_key_to_d];
                    (*current_node)->_values[index_of_deleted] = (*suitable_node)->_values[index_closest_to_key_to_d];

                    (*suitable_node)->_keys[index_closest_to_key_to_d] = tmp_k;

                    // push current node and ex-position of node to delete to stack
                    path.push(std::pair< node **, unsigned >(current_node, index_closest_to_key_to_d));
                    // inner_remove_node(path, sub node, ex-closest_key position)
                    remove_inner(path, suitable_node, index_closest_to_key_to_d);
                } else {
                    // merge sub nodes
                    this->debug_with_guard("b_tree::removing_template_method::remove_inner there are no sub trees with count > " +
                                                   std::to_string(min_count_of_keys_in_node) + ". Need to merge sub-nodes");

                    // insert d in merged sub nodes
                    unsigned index_of_deleted_element_in_left_subtree = (*left_sub_node)->_number_of_keys;
                    (*left_sub_node)->_keys[(*left_sub_node)->_number_of_keys] = (*current_node)->_keys[index_of_deleted];
                    (*left_sub_node)->_number_of_keys++;
                    memmove(((*left_sub_node)->_keys + (*left_sub_node)->_number_of_keys), (*right_sub_node)->_keys, sizeof(tkey) * ((*right_sub_node)->_number_of_keys));
                    memmove(((*left_sub_node)->_values + (*left_sub_node)->_number_of_keys), (*right_sub_node)->_values, sizeof(tvalue) * ((*right_sub_node)->_number_of_keys));
                    memmove(((*left_sub_node)->_sub_nodes + (*left_sub_node)->_number_of_keys + 1), (*right_sub_node)->_sub_nodes, sizeof(tkey) * ((*right_sub_node)->_number_of_keys + 1));
                    (*left_sub_node)->_number_of_keys += (*right_sub_node)->_number_of_keys;

                    // remove right subtree
                    this->cleanup_node(right_sub_node);

                    // if count of keys in current node is 1, merged must replace current node
                    if ((*current_node)->_number_of_keys == 1) {
                        // todo: a problem. we cleanup current node before assigning to it a left sub node, so info about left sub node is lost. we either should remove one *, either should (try) saving (*current node) somewhere. Second variant probably not works...
                        this->debug_with_guard("b_tree::removing_template_method::remove_inner replace current node with left subtree node");
//                        node * tmp = (*current_node);
                        (*current_node) = (*left_sub_node);
                        this->cleanup_node(current_node);
                        // path.pop() // ?
                    }
                    else {
                        // move elements
                        delete_key_value_from_node((*current_node), index_of_deleted, false);
                    }

                    // inner_remove_node(path, sub node, ex-closest_key position)
                    remove_inner(path, left_sub_node, index_of_deleted_element_in_left_subtree);
                }
            }
            this->trace_with_guard("b_tree::removing_template_method::remove_inner method started");
        }

        virtual tvalue remove(tkey const &key)
        {
            this->trace_with_guard("bs_tree::removing_template_method::remove method started");

            if (this->_target_tree->_root == nullptr) {
                this->warning_with_guard("bs_tree::removing_template_method::remove tree is empty")
                    ->trace_with_guard("bs_tree::removing_template_method::remove method finished");
                throw remove_exception("bs_tree::removing_template_method::remove tree is empty");
            }

            // returned by find path is std::pair<std::stack<node **>, std::pair<node **, unsigned>>
            auto path_and_target = this->find_path(key);
            std::stack<std::pair< node **, unsigned >> path = path_and_target.first;
            node **target_ptr = path_and_target.second.first;
            unsigned target_index = path_and_target.second.second;
            tkey_comparer comparer;
            auto comparison_result = comparer(key, (*target_ptr)->_keys[target_index]);

            if (comparison_result != 0)
            {
                this->warning_with_guard("b_tree::removing_template_method::remove passed key is not found")
                    ->trace_with_guard("b_tree::removing_template_method::remove method finished");
                throw remove_exception("b_tree::removing_template_method::remove passed key is not found");
            }

            tvalue result = (*target_ptr)->_values[target_index];

            this->debug_with_guard("before remove_inner:: current node count: " + std::to_string((*target_ptr)->_number_of_keys) + " index: " +
                                           std::to_string(target_index));
            remove_inner(path, target_ptr, target_index);

            this->trace_with_guard("b_tree::removing_template_method::remove method finished");
            return result;
        }

    protected:
        virtual void after_remove(
                std::stack<node **> &path) const
        {

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
public:
    void insert(tkey const &key, tvalue &&value) override
    {
        this->_insertion->insert(key, value);
//        this->_insertion->insert(key, std::move(value));
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
    b_tree(unsigned tree_parameter,
           logger *logger,
           memory *allocator,
           insertion_template_method *insertion,
           finding_template_method *finding,
           removing_template_method *removing)
    : _tree_parameter(tree_parameter),
      _logger(logger),
      _allocator(allocator),
      _insertion(insertion),
      _finding(finding),
      _removing(removing),
      _root(nullptr)
    {

    }

public:
    // public constructor
    explicit b_tree(unsigned tree_parameter, logger *logger = nullptr, memory *allocator = nullptr)
    : b_tree(tree_parameter,
             logger,
             allocator,
             new insertion_template_method(this),
             new finding_template_method(this),
             new removing_template_method(this))
    {
        if (tree_parameter < 2) {
            throw b_tree_exception("b_tree::tree parameter cannot be less than 3");
        }
    }

public:
    // copy constructor
    b_tree(b_tree<tkey, tvalue, tkey_comparer> const &obj)
    : b_tree(obj._tree_parameter, obj._logger, obj._allocator)
    {
        _root = copy(obj._root);
    }

    // move constructor
    b_tree(b_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
    : b_tree(obj._tree_parameter,
            obj._logger,
            obj._allocator,
            obj._insertion,
            obj._finding,
            obj._removing)
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

        _tree_parameter = obj._tree_parameter;
        _logger = obj._logger;
        _allocator = obj._allocator;

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

        _tree_parameter = obj._tree_parameter;

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
