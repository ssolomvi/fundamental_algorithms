#include "binary_search_tree.h"

#pragma region iterators
#pragma region preorder iterator
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::prefix_iterator(
        binary_search_tree::node *current_node)
        : _current_node(current_node)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator==(
        const binary_search_tree::prefix_iterator &other) const
{
    return (_current_node == other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator!=(
        const binary_search_tree::prefix_iterator &other) const
{
    return (_current_node != other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++()
{
    if (_current_node == nullptr) {
        // TODO: throw an exception
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
        }
        else if (_path.top()->left_subtree_address == _current_node) {
            while (true) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    break;
                }

                _current_node = _path.top();
                if (_current_node->right_subtree_address != nullptr) {
                    _current_node = _current_node->right_subtree_address;
                    break;
                } else {
                    _path.pop();
                }
            }
        }
        else if (_path.top()->right_subtree_address == _current_node) {
            while (true) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    break;
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
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++(int not_used)
{
    auto result = *this;
    ++*this;
    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const &, tvalue const &>
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator*() const
{
    return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
}
#pragma endregion

#pragma region inorder iterator
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::infix_iterator(
        binary_search_tree::node *current_node)
        : _current_node(current_node)
{
    if (_current_node == nullptr) {
        return;
    }

    while (_current_node->left_subtree_address != nullptr) {
        _path.push(_current_node);
        _current_node = _current_node->left_subtree_address;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator==(
        const binary_search_tree::infix_iterator &other) const
{
    return (_current_node == other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator!=(
        const binary_search_tree::infix_iterator &other) const
{
    return (_current_node != other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++()
{
    // левое--корень--правое
    if (_current_node == nullptr) {
        // TODO: throw
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
                break;
            }

            _current_node = _path.top();
            _path.pop();
        }

        if (!_path.empty()) {
            _current_node = _path.top();
            _path.pop();
        }
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++(int not_used)
{
    auto result = *this;
    ++*this;
    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const &, tvalue const &>
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator*() const {
    return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
}
#pragma endregion

#pragma region postorder iterator
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::postfix_iterator(
        binary_search_tree::node *current_node)
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
        const binary_search_tree::postfix_iterator &other) const
{
    return (_current_node == other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator!=(
        const binary_search_tree::postfix_iterator &other) const
{
    return (_current_node != other._current_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++()
{
    // если можно идти налево, идём, если можно направо, идём
    if (_current_node == nullptr) {
        // TODO: throw
    }

    if (_path.empty()) // всё обошли
    {
        _current_node == nullptr;
    }

    // поднимаемся слева
    else if (_path.top()->left_subtree_address == _current_node) {
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
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++(int not_used)
{
    auto result = *this;
    ++*this;
    return *this;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
std::tuple<unsigned int, tkey const &, tvalue const &>
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator*() const {
    return std::tuple<unsigned int, const tkey &, const tvalue &>(_path.size(), _current_node->key, _current_node->value);
}
#pragma endregion

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_prefix() const noexcept
{
    return binary_search_tree::prefix_iterator(_root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_prefix() const noexcept
{
    return binary_search_tree::prefix_iterator(nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_infix() const noexcept
{
    return binary_search_tree::infix_iterator(_root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_infix() const noexcept
{
    return binary_search_tree::infix_iterator(nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_postfix() const noexcept
{
    return binary_search_tree::postfix_iterator(_root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_postfix() const noexcept
{
    return binary_search_tree::postfix_iterator(nullptr);
}

#pragma endregion

#pragma region template methods

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::template_method_basics(
        binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : _target_tree(target_tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::pair<std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>, typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **>
binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::find_path(const tkey &key)
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
            return { path, iterator };
        }

        path.push(iterator);
        iterator = comparison_result < 0
                   ? &(*iterator)->left_subtree
                   : &(*iterator)->right_subtree;
    }

    return { path, iterator };
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::template_method_basics::get_logger() const noexcept
{
    return _target_tree->get_logger();
}

#pragma region insertion template method
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insertion_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void
binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insert(const tkey &key, tvalue &&value)
{
    auto path_and_target = this->find_path(key);
    auto path = path_and_target.first;
    auto **target_ptr = path_and_target.second;

    if (*target_ptr != nullptr)
    {
        // TODO: exception || update value
    }

    *target_ptr = reinterpret_cast<node *>(allocate_with_guard(get_node_size()));
    initialize_memory_with_node(*target_ptr);

    (*target_ptr)->key = key;
    (*target_ptr)->value = std::move(value);
    (*target_ptr)->left_subtree = nullptr;
    (*target_ptr)->right_subtree = nullptr;

    after_insert_inner(path, target_ptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
size_t binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::get_node_size() const
{
    return sizeof(node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::initialize_memory_with_node(
        binary_search_tree::node *target_ptr) const
{
    new(target_ptr) node;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::after_insert_inner(
        std::stack<node * *> &path, binary_search_tree::node **target_ptr)
{
    // TODO: nothing to do here in BST context...
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::get_memory() const noexcept
{
    return this->_target_tree->_allocator;
}

#pragma endregion

#pragma region finding template method
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::finding_template_method::finding_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue const &binary_search_tree<tkey, tvalue, tkey_comparer>::finding_template_method::find(const tkey &key)
{
    auto path_and_target = this->find_path(key);
    auto path = path_and_target.first;
    auto **target_ptr = path_and_target.second;

    if (*target_ptr == nullptr)
    {
        // TODO: exception
    }

    after_find_inner(path, target_ptr);

    return (*target_ptr)->value;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void
binary_search_tree<tkey, tvalue, tkey_comparer>::finding_template_method::after_find_inner(std::stack<node * *> &path, binary_search_tree::node **target_ptr)
{
    // TODO: nothing to do here in BST context...
}

#pragma endregion

#pragma region removing template method
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::removing_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue &&binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::remove(const tkey &key)
{
    auto path_and_target = this->find_path(key);
    auto path = path_and_target.first;
    auto **target_ptr = path_and_target.second;

    if (*target_ptr == nullptr)
    {
        // TODO: exception
    }

    tvalue &&result = std::move((*target_ptr)->value);

    if ((*target_ptr)->left_subtree != nullptr &&
        (*target_ptr)->right_subtree != nullptr)
    {
        auto **element_to_swap_with = &(*target_ptr)->left_subtree;

        while ((*element_to_swap_with)->right_subtree != nullptr)
        {
            path.push(element_to_swap_with);
            element_to_swap_with = &(*element_to_swap_with)->right_subtree;
        }

        swap_nodes(element_to_swap_with, target_ptr);
    }

    if ((*target_ptr)->left_subtree == nullptr &&
        (*target_ptr)->right_subtree == nullptr)
    {
        cleanup_node(target_ptr);
    }
    else if ((*target_ptr)->left_subtree != nullptr)
    {
        auto *target_left_subtree = (*target_ptr)->left_subtree;
        cleanup_node(target_ptr);
        *target_ptr = target_left_subtree;
    }
    else
    {
        auto *target_right_subtree = (*target_ptr)->right_subtree;
        cleanup_node(target_ptr);
        *target_ptr = target_right_subtree;
    }

    after_remove(path);

    return std::move(result);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
template<typename T>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::swap(T **left, T **right)
{
    T *temp = *left;
    *left = *right;
    *right = temp;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::swap_nodes(
        binary_search_tree::node **one_node, binary_search_tree::node **another_node)
{
    swap(&(*one_node)->left_subtree, &(*another_node)->left_subtree);
    swap(&(*one_node)->right_subtree, &(*another_node)->right_subtree);

    swap_additional_data(*one_node, *another_node);

    swap(one_node, another_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::cleanup_node(
        binary_search_tree::node **node_address)
{
    (*node_address)->~node();
    deallocate_with_guard(reinterpret_cast<void *>(*node_address));

    *node_address = nullptr;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::swap_additional_data(
        binary_search_tree::node *one_node, binary_search_tree::node *another_node)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::after_remove(
        const std::stack<node **> &path) {

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::get_memory() const noexcept
{
    return this->_target_tree->_allocator;
}

#pragma endregion

#pragma endregion

#pragma region rool 5
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        const binary_search_tree<tkey, tvalue, tkey_comparer> &obj)
        : binary_search_tree(obj._logger, obj._allocator)
{
    _root = copy(obj._root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
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

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer> &
binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(const binary_search_tree<tkey, tvalue, tkey_comparer> &obj)
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

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer> &
binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(binary_search_tree<tkey, tvalue, tkey_comparer> &&obj) noexcept
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

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::~binary_search_tree()
{
    delete _insertion;
    delete _finding;
    delete _removing;

    clear_up(_root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::clear_up(binary_search_tree::node *element)
{
    if (element == nullptr)
    {
        return;
    }

    clear_up(element->left_subtree);
    clear_up(element->right_subtree);

    element->~node();
    deallocate_with_guard(element);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *binary_search_tree<tkey, tvalue, tkey_comparer>::copy(binary_search_tree::node *from)
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

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        logger *logger, memory *allocator,
        binary_search_tree::insertion_template_method *insertion,
        binary_search_tree::finding_template_method *finding,
        binary_search_tree::removing_template_method *removing)
        : _logger(logger),
          _allocator(allocator),
          _insertion(insertion),
          _finding(finding),
          _removing(removing),
          _root(nullptr)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(logger *logger, memory *allocator)
        : binary_search_tree(logger,
                             allocator,
                             new insertion_template_method(this),
                             new finding_template_method(this),
                             new removing_template_method(this))
{

}

#pragma endregion

#pragma region associative container contract
template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insert(const tkey &key, tvalue &&value)
{
    _insertion->insert(key, std::move(value));
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue const &binary_search_tree<tkey, tvalue, tkey_comparer>::get(const tkey &key)
{
    return _finding->find(key);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue &&binary_search_tree<tkey, tvalue, tkey_comparer>::remove(const tkey &key)
{
    return std::move(_removing->remove(key));
}

#pragma endregion

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::get_memory() const noexcept
{
    return _allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::get_logger() const noexcept
{
    return _logger;
}
