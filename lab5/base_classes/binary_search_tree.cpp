#include "binary_search_tree.h"

// region iterators implementation

//region prefix_iterator implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::prefix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *current_node)
        : _current_node(current_node)
{
    // TODO: what should happen here, when 'root' is nullptr
    if (_current_node == nullptr) {
        return;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator==(
        binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator const &other) const
{
    // TODO: _path == other._path?
    return (_iterable_context == other._iterable_context && _current_node == other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator!=(
        const binary_search_tree::prefix_iterator &other) const {
    // TODO: _path != other._path?
    return (_iterable_context != other._iterable_context || _current_node != other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator&
         binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++()
{
    if (_current_node == nullptr) {
        return;
    }

    if (_current_node->left_subtree_address != nullptr) {
        _path.push(_current_node);
        _current_node = _current_node->left_subtree_address;
    }
    else if (_current_node->right_subtree_address != nullptr) {
        _path.push(_current_node);
        _current_node = _current_node->right_subtree_address;
    }
    else {
        if (_path.empty()) {
            _current_node = nullptr;
            return;
        }

        if (_path.top()->left_subtree_address == _current_node) {
            while (true) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    return;
                }

                _current_node = _path.top();
                if (_current_node->right_subtree_address != nullptr) {
                    _current_node = _current_node->right_subtree_address;
                    break;
                } else {
                    _path.pop();
                }
            }
        } else if (_path.top()->right_subtree_address == _current_node) {
            while (true) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    return;
                }

                _current_node = _path.top();
                _path.pop();
                if (_path.top()->left_subtree_address == _current_node && _path.top()->right_subtree_address != nullptr) {
                    _current_node = _path.top()->right_subtree_address;
                    break;
                }
            }
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator
         binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++(
        int not_used)
{
    auto result = *this;
    ++*this;
    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue const&>
        binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator*() const
{
    // TODO: why unsigned int needed
    unsigned int something;
    return std::tuple<unsigned int, tkey const&, tvalue const&>{something, _current_node->key, _current_node->value};
}

//endregion prefix_iterator implementation

//region infix_iterator implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::infix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *current_node)
        : _current_node(current_node)
{
    if (_current_node == nullptr) {
        return;
    }

    while (_current_node->left_subtree_address != nullptr) {
        _path.push(_current_node);
        _current_node = _current_node->left_subtree_address;
    }
    /*
    InorderIterator(Node *node) {
        Node *current = node;
        while (NULL != current) {
            mStack.push(current);
            current = current->left;
        }
    }
    */
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator==(
        binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator const &other) const
{
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator &
         binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++()
{
    // левое--корень--правое
    if (_current_node == nullptr) {
        return;
    }

    if (_current_node->right_subtree_address != nullptr) {
        _path.push(_current_node);
        _current_node = _current_node->right_subtree_address;
        while (_current_node->left_subtree_address != nullptr) {
            _path.push(_current_node);
            _current_node = _current_node->left_subtree_address;
        }
    }
    else if (_path.top()->left_subtree_address == _current_node) {
        _current_node = _path.top();
        _path.pop();
    }
    else if (_path.top()->right_subtree_address == _current_node) {
        while (_path.top()->right_subtree_address == _current_node) {
            if (_path.empty()) {
                _current_node = nullptr;
                return;
            }

            _current_node = _path.top();
            _path.pop();
        }
        _current_node = _path.top();
        _path.pop();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator
         binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++(
        int not_used)
{
    auto result = *this;
    ++*this;
    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue const&> binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator*() const
{

}

//endregion infix_iterator implementation

//region postfix_iterator implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::postfix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *current_node)
        : _current_node(current_node)
{
    // лево-право-текущий
    if (_current_node == nullptr) {
        return;
    }

    while (_current_node->right_subtree_address || _current_node->left_subtree_address) {
        _path.push(_current_node);
        _current_node = _current_node->left_subtree_address != nullptr ?
                _current_node->left_subtree_address :
                _current_node->right_subtree_address;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator==(
        binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator const &other) const
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator &
         binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++() // префиксный ++iterator
{
    // если можно идти налево, идём, если можно направо, идём
    if (_path.empty()) // всё обошли
    {
        _current_node == nullptr;
        return;
    }

    // поднимаемся слева
    if (_path.top()->left_subtree_address == _current_node) {
        if (_path.top()->right_subtree_address != nullptr) {
            _current_node = _path.top()->right_subtree_address;
            while (_current_node->right_subtree_address || _current_node->left_subtree_address) {
                _path.push(_current_node);
                _current_node = _current_node->left_subtree_address != nullptr ?
                        _current_node->left_subtree_address :
                        _current_node->right_subtree_address;
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
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator
         binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++(
        int not_used) // постфиксный итератор
{
    auto result = *this;
    ++*this;
    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue const&>
        binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator*() const
{
// вызывать contructor, не make_tuple!!
}

//endregion prefix_iterator implementation

// endregion iterators implementation

// region template methods implementation

template<typename tkey, typename tvalue, typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::template_methods::
find_a_node_place(
        const tkey &key,
        binary_search_tree::node *&tree_root_address,
        std::stack<node **> &path_to_subtree_root_exclusive,
        binary_search_tree::template_methods::was_found_find_a_node_place_function_response *is_found,
        binary_search_tree::template_methods::direction_find_a_node_place_function_response *direction)
{
    *is_found = not_found;
    *direction = root;
    if (this->_context_tree->_root == nullptr) {
        return;
    }

    auto * current_node = tree_root_address;
    int comparison_result = 0;
    while (current_node != nullptr) {
        comparison_result = tkey_comparer(key, current_node->key);
        if (comparison_result == 0) {
            *is_found = found;
            return;
        } else {
            path_to_subtree_root_exclusive.push(&current_node);
            if (comparison_result < 0) {
                *direction = left_subtree;
                current_node = current_node->left_subtree_address;
            } else {
                *direction = right_subtree;
                current_node = current_node->right_subtree_address;
            }
        }
    }
}

// region insertion implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insert(
        tkey const &key,
        tvalue &&value,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&tree_root_address,
        binary_search_tree<tkey, tvalue, tkey_comparer> * context_tree)
{
    this->_context_tree = context_tree;
    return insert_inner(key, value, tree_root_address, std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **>());
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insert_inner(
        tkey const &key,
        tvalue &&value,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive)
{
    before_insert_inner(key, std::move(value), subtree_root_address, path_to_subtree_root_exclusive);

    after_insert_inner(key, std::move(value), subtree_root_address, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::before_insert_inner(
        tkey const &key,
        tvalue &&value,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive)
{
    // todo: памагити позязя
    typename template_methods::was_found_find_a_node_place_function_response is_found;
    typename template_methods::direction_find_a_node_place_function_response direction;
    find_a_node_place(key, subtree_root_address, path_to_subtree_root_exclusive, &is_found, &direction);

    if (is_found == template_methods::was_found_find_a_node_place_function_response::found) {
        throw bst_exception("Passed tkey is already present in tree");
    }

    if (direction == template_methods::direction_find_a_node_place_function_response::right_subtree) {
        initialize_memory_with_node(*(path_to_subtree_root_exclusive.top())->right_subtree_address);
    } else if (direction == template_methods::direction_find_a_node_place_function_response::left_subtree) {
        initialize_memory_with_node(*(path_to_subtree_root_exclusive.top())->left_subtree_address);
    } else {
        initialize_memory_with_node(template_methods::_context_tree->_root);
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::after_insert_inner(
        tkey const &key,
        tvalue &&value,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive,
        typename template_methods::inner_function_response response)
{
// баланс узлов
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
::initialize_memory_with_node(tkey const &key,
                              tvalue &&value,
                              binary_search_tree<tkey, tvalue, tkey_comparer>::node * const node_address)
{
    this->trace_with_guard("Method 'initialize_memory_with_node' execution started");
    // TODO: ask about it
    new (node_address) binary_search_tree<tkey, tvalue, tkey_comparer>::node;

    node_address->right_subtree_address = nullptr;
    node_address->left_subtree_address = nullptr;
    node_address->key = key;
    node_address->value = std::move(value);

    this->trace_with_guard("Method 'initialize_memory_with_node' execution finished");
}

// endregion insertion implementation

// region reading implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue const &binary_search_tree<tkey, tvalue, tkey_comparer>::reading_template_method::read(
        tkey const &key,
        node *&tree_root_address,
        binary_search_tree<tkey, tvalue, tkey_comparer> * context_tree)
{
    this->_context_tree = context_tree;
    return read_inner(key, tree_root_address, std::stack<node **>());
}

template<typename tkey, typename tvalue, typename tkey_comparer>
tvalue const &binary_search_tree<tkey, tvalue, tkey_comparer>::reading_template_method::read_inner(
        tkey const &key,
        node *&subtree_root_address,
        std::stack<node **> &path_to_subtree_root_exclusive)
{
    typename template_methods::was_found_find_a_node_place_function_response is_found;
    before_read_inner(key, subtree_root_address, path_to_subtree_root_exclusive, &is_found);

    if (is_found == template_methods::was_found_find_a_node_place_function_response::not_found) {
        // TODO: response
        this->information_with_guard("The value with such key not found");
    } else {
        // TODO: is this right?
        this->information_with_guard("The value with such key was found successfully");
        return (*(path_to_subtree_root_exclusive.top()))->value;
    }
    after_read_inner(key, subtree_root_address, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::reading_template_method::before_read_inner(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::template_methods::was_found_find_a_node_place_function_response *is_found)
{
    typename template_methods::direction_find_a_node_place_function_response direction;

    template_methods::find_a_node_place(key, subtree_root_address, path_to_subtree_root_exclusive, is_found, &direction);

    if (*is_found == template_methods::was_found_find_a_node_place_function_response::found) {
        if (direction == template_methods::direction_find_a_node_place_function_response::left_subtree) {
            path_to_subtree_root_exclusive.push(&((*(path_to_subtree_root_exclusive.top()))->left_subtree_address));
        } else if (direction == template_methods::direction_find_a_node_place_function_response::right_subtree) {
            path_to_subtree_root_exclusive.push(&((*(path_to_subtree_root_exclusive.top()))->right_subtree_address));
        } else {
            path_to_subtree_root_exclusive.push(&(subtree_root_address));
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::reading_template_method::after_read_inner(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive)
{

}

// endregion reading implementation

// region removing implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue &&binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::remove(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&tree_root_address,
        binary_search_tree<tkey, tvalue, tkey_comparer> * context_tree)
{
    this->_context_tree = context_tree;
    return std::move(remove_inner(key, tree_root_address, std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **>()));
}

template<typename tkey, typename tvalue, typename tkey_comparer>
tvalue &&binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::remove_inner(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive)
{
    // find a node to delete
    typename template_methods::was_found_find_a_node_place_function_response is_found;
    before_remove_inner(key, subtree_root_address, path_to_subtree_root_exclusive, &is_found);
    // stack has all nodes to node_to_delete, excluding the node_to_delete

    tvalue && to_return = 0;

    if (is_found == template_methods::was_found_find_a_node_place_function_response::not_found) {
        // TODO: handle response from found
    } else {
        subtree_root_address = (*path_to_subtree_root_exclusive.top());
        to_return = (*path_to_subtree_root_exclusive.top())->value;
        path_to_subtree_root_exclusive.pop();
    }

    after_remove_inner(key, subtree_root_address, path_to_subtree_root_exclusive);
    return to_return;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::before_remove_inner(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::template_methods::was_found_find_a_node_place_function_response *is_found)
{
    typename template_methods::direction_find_a_node_place_function_response direction;

    template_methods::find_a_node_place(key, subtree_root_address, path_to_subtree_root_exclusive, is_found, &direction);

    if (*is_found == template_methods::was_found_find_a_node_place_function_response::found) {
        if (direction == template_methods::direction_find_a_node_place_function_response::left_subtree) {
            path_to_subtree_root_exclusive.push(&((*(path_to_subtree_root_exclusive.top()))->left_subtree_address));
        } else if (direction == template_methods::direction_find_a_node_place_function_response::right_subtree) {
            path_to_subtree_root_exclusive.push(&((*(path_to_subtree_root_exclusive.top()))->right_subtree_address));
        } else {
            path_to_subtree_root_exclusive.push(&(subtree_root_address));
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::after_remove_inner(
        tkey const &key,
        binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
        std::stack<binary_search_tree<tkey, tvalue, tkey_comparer>::node **> &path_to_subtree_root_exclusive,
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::template_methods::direction_find_a_node_place_function_response &direction)
{
    // stack.top() is a parent of to_delete node
    // subtree_root_address is a node to delete
    if (path_to_subtree_root_exclusive.empty()
        && subtree_root_address->left_subtree_address == nullptr
        && subtree_root_address->right_subtree_address == nullptr) {
        // root was just deleted from tree
        // TODO: redo links in binary tree
    }

    node * to_delete = subtree_root_address, * to_replace = nullptr, * parent_to_to_replace = nullptr;

    // there are 2 children
    if (to_delete->right_subtree_address && to_delete->left_subtree_address) {
        // go left, take the most right
        to_replace = to_delete->left_subtree_address;
        while (to_replace->right_subtree_address != nullptr) {
            parent_to_to_replace = to_replace;
            to_replace = to_replace->right_subtree_address;
        }

        if (to_replace->left_subtree_address != nullptr) {
            parent_to_to_replace->right_subtree_address = to_replace->left_subtree_address;
        }
    }
    // there is 1 child in the left
    else if (to_delete->left_subtree_address != nullptr) {
        to_replace = to_delete->left_subtree_address;
    }
    // there is 1 child in the right
    else if (to_delete->right_subtree_address != nullptr) {
        to_replace = to_delete->right_subtree_address;
    }
    // node to_delete is a list, no replacing required
    else {
        return;
    }

    if (direction == template_methods::direction_find_a_node_place_function_response::root) {
        // TODO: попортить указатель на корень дерева в классе дерева
    } else if (direction == template_methods::direction_find_a_node_place_function_response::left_subtree) {
        (*(path_to_subtree_root_exclusive.top()))->left_subtree_address = to_replace;
    } else {
        (*(path_to_subtree_root_exclusive.top()))->right_subtree_address = to_replace;
    }
}

// endregion implementation

// endregion template methods

// region construction, assignment, destruction implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        binary_search_tree::insertion_template_method *insertion,
        binary_search_tree::reading_template_method *reading,
        binary_search_tree::removing_template_method *removing,
        memory *allocator,
        logger *logger)
        : _insertion(insertion),
          _reading(reading),
          _removing(removing),
          _allocator(allocator),
          _logger(logger)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        memory *allocator,
        logger *logger):
        _insertion(new insertion_template_method()),
        _reading(new reading_template_method()),
        _removing(new removing_template_method()),
        _allocator(allocator),
        _logger(logger),
        _root(nullptr)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        const binary_search_tree &other)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        binary_search_tree &&other) noexcept
{
    // TODO
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer> &binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        const binary_search_tree &other)
{
    if (this == &other)
    {
        return *this;
    }



    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer> &binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        binary_search_tree &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }



    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::~binary_search_tree()
{
    // TODO: remove all elements

    delete _insertion;
    delete _reading;
    delete _removing;
}

// endregion construction, assignment, destruction implementation

// region associative_container contract implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insert(
        tkey const &key,
        tvalue &&value)
{
    // TODO: ask if it is right
    return _insertion->insert(key, std::move(value), _root, this);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue const &binary_search_tree<tkey, tvalue, tkey_comparer>::get(
        tkey const &key)
{
    return _reading->read(key, _root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue &&binary_search_tree<tkey, tvalue, tkey_comparer>::remove(
        tkey const &key)
{
    return std::move(_removing->remove(key, _root));
}

// endregion associative_container contract implementation

// region logger_holder contract implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::get_logger() const noexcept
{
    return _logger;
}

// endregion logger_holder contract implementation

// region iterators requesting implementation

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_prefix() const noexcept
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_prefix() const noexcept
{
// constructor interator (this, nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_infix() const noexcept
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_infix() const noexcept
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_postfix() const noexcept
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_postfix() const noexcept
{

}

/*
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::string binary_search_tree<tkey, tvalue, tkey_comparer>::get_typename() noexcept {
    auto tkey_typename = typeid(tkey).name();
    auto tvalue_typename = typeid(tvalue).name();
    auto tkey_comparer_typename = typeid(tkey_comparer).name();

    return std::string("binary_search_tree<" + tkey_typename + ", " + tvalue_typename + ", " + tkey_comparer_typename);
}
*/

// endregion iterators requesting implementation
