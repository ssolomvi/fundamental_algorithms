#include "data_base.h"

#pragma region Find structure

associative_container<std::string, associative_container<std::string, associative_container<key, db_value *> *> *> *
data_base::find_data_pool(const std::string &pool_name) const
{
    this->trace_with_guard("data_base::find_data_pool method started");
    if (pool_name.empty()) {
        this->warning_with_guard("data_base::find_data_pool pool name must not be an empty string")
                ->trace_with_guard("data_base::find_data_pool method finished");
        throw data_base::db_find_exception("find_data_pool:: pool name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *> *> *data_pool;

    try {
        data_pool = this->_database->get(pool_name);
    }
    catch (bs_tree<std::string,
            associative_container<std::string,
                associative_container<std::string,
                    associative_container<key, db_value *> *> *> *, data_base::string_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_data_pool data pool not found")
                ->trace_with_guard("data_base::find_data_pool method finished");
        throw data_base::db_find_exception("find_data_pool:: data pool not found");
    }

    this->trace_with_guard("data_base::find_data_pool method finished");
    return data_pool;
}

associative_container<std::string, associative_container<key, db_value *> *> *
data_base::find_data_scheme(const std::string &pool_name, const std::string &scheme_name) const
{
    this->trace_with_guard("data_base::find_data_scheme method started");
    if (scheme_name.empty()) {
        this->warning_with_guard("data_base::find_data_scheme scheme name must not be an empty string")
                ->trace_with_guard("data_base::find_data_scheme method finished");
        throw data_base::db_find_exception("find_data_scheme:: scheme name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *> *> *data_pool = find_data_pool(pool_name);

    associative_container<std::string,
            associative_container<key, db_value *> *> *data_scheme;

    try {
        data_scheme = data_pool->get(scheme_name);
    }
    catch (typename bs_tree<std::string,
            associative_container<std::string,
                associative_container<key, db_value *> *> *, data_base::string_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_data_scheme data scheme not found")
                ->trace_with_guard("data_base::find_data_scheme method finished");
        throw data_base::db_find_exception("find_data_scheme:: data scheme not found");
    }

    this->trace_with_guard("data_base::find_data_scheme method finished");
    return data_scheme;
}

associative_container<key, db_value *> *
data_base::find_data_collection(const std::string &pool_name,
                                const std::string &scheme_name,
                                const std::string &collection_name) const
{
    this->trace_with_guard("data_base::find_data_collection method started");

    if (collection_name.empty()) {
        this->warning_with_guard("data_base::find_data_collection collection name must not be an empty string")
                ->trace_with_guard("data_base::find_data_collection method finished");
        throw data_base::db_find_exception("find_data_collection:: collection name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<key, db_value *> *> *data_scheme = find_data_scheme(pool_name, scheme_name);

    associative_container<key, db_value *> *data_collection;

    try {
        data_collection = data_scheme->get(collection_name);
    }
    catch (typename bs_tree<std::string, associative_container<key, db_value *> *, data_base::string_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_data_collection data collection not found")
                ->trace_with_guard("data_base::find_data_collection method finished");
        throw data_base::db_find_exception("find_data_collection:: data collection not found");
    }

    this->trace_with_guard("data_base::find_data_collection method finished");
    return data_collection;
}

#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection

void
data_base::add_to_collection(const std::string &pool_name,
                             const std::string &scheme_name,
                             const std::string &collection_name, const key& _key,
                             db_value * value) const
{
    this->trace_with_guard("data_base::add_to_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);
    // handle if not found ?
    db_value * found_value;
    try {
        found_value = data_collection->get(_key);
        handler *add_handle = new add_handler(value);
        handler *last_handler = found_value->get_last_handler();
        if (last_handler == nullptr) {
            // there are no commands added
            this->warning_with_guard("data_base::add_to_collection add handler cannot be the first one in chain of responsibility")
                    ->trace_with_guard("data_base::add_to_collection method finished");
            throw handler::order_exception("add_to_collection:: add handler cannot be the first one in chain of responsibility");
        } else {
            // add command may be only after remove command
            if (last_handler->get_handler_type() == handler::handler_types::_remove_handler_) {
                found_value->add_new_handler(add_handle);
            } else {
                this->warning_with_guard("data_base::add_to_collection add handler can be only after remove_handler in chain of responsibility")
                        ->trace_with_guard("data_base::add_to_collection method finished");
                throw handler::order_exception("add_to_collection:: add handler can be only after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        data_collection->insert(_key, std::move(value));
    }
    this->trace_with_guard("data_base::add_to_collection method finished");
}

#pragma endregion

#pragma region Updating a collection value
void
data_base::update_in_collection(const std::string &pool_name, const std::string &scheme_name,
                                const std::string &collection_name, const key& _key,
                                std::map<db_value_fields, unsigned char *> upd_dict) const
{
    this->trace_with_guard("data_base::update_in_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);
    // handle if not found ?
    db_value * found_value;
    try {
        found_value = data_collection->get(_key);
        handler * update_handle = new update_handler(std::move(upd_dict));
        handler * last_handler = found_value->get_last_handler();
        if (last_handler == nullptr) {
            found_value->add_new_handler(update_handle);
        } else {
            if (last_handler->get_handler_type() != handler::handler_types::_remove_handler_) {
                found_value->add_new_handler(update_handle);
            } else {
                this->warning_with_guard("data_base::update_in_collection update handler cannot be after remove_handler in chain of responsibility")
                        ->trace_with_guard("data_base::update_in_collection method finished");
                throw handler::order_exception("update_in_collection:: update handler cannot be after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::update_in_collection no element with passed _key found")
                ->trace_with_guard("data_base::update_in_collection method finished");
        throw data_base::db_find_exception("update_in_collection:: no element with passed _key found");
    }

    this->trace_with_guard("data_base::update_in_collection method finished");
}

#pragma endregion

#pragma region Finding among collection
db_value *
data_base::find_among_collection(const std::string &pool_name, const std::string &scheme_name,
                                 const std::string &collection_name, const key& _key) const
{
    this->trace_with_guard("data_base::find_among_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);

    db_value * found_value;
    try {
        found_value = data_collection->get(_key);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_among_collection no value with passed _key found")
                ->trace_with_guard("data_base::find_among_collection method finished");
        throw data_base::db_find_exception("find_among_collection:: no value with passed _key found");
    }

    this->trace_with_guard("data_base::find_among_collection method finished");
    return found_value;
}

std::vector<db_value *>
data_base::find_dataset_with_time
(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name,
 key min_key, key max_key, uint64_t time_parameter)
{
    this->trace_with_guard("data_base::find_dataset_with_time method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);

    key_comparer comparer;
    auto comparison_result = comparer(min_key, max_key);
    if (comparison_result > 0) {
        key tmp = min_key;
        min_key = max_key;
        max_key = tmp;
    }

    std::vector<db_value *> to_return_vector;

    if (comparison_result == 0) {
        try {
            db_value * found = find_with_time(pool_name, scheme_name, collection_name, nullptr, min_key, time_parameter);
            to_return_vector.push_back(found);
            return to_return_vector;
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
            throw db_find_exception("data_base::find_in_range no appropriate values found in collection");
        }
    }

    // done only for bst-like trees. B and B+ trees require another iterators
    auto * bst = reinterpret_cast<bs_tree<key, db_value *, key_comparer> *>(data_collection);
    auto end_iteration = bst->end_infix();
    bool in_range = false;

    for (auto it = bst->begin_infix(); it != end_iteration; ++it) {
        if (in_range) {
            if (comparer(max_key, std::get<1>(*it)) >= 0) {
                to_return_vector.push_back(find_with_time(pool_name, scheme_name, collection_name, std::get<2>(*it), std::get<1>(*it), time_parameter));
            } else {
                break;
            }
        }

        if (!in_range) {
            if (comparer(min_key, std::get<1>(*it)) <= 0) {
                in_range = true;
                to_return_vector.push_back(find_with_time(pool_name, scheme_name, collection_name, std::get<2>(*it), std::get<1>(*it), time_parameter));
            }
        }
    }

    this->trace_with_guard("data_base::find_dataset_with_time method finished");
    return to_return_vector;
}

db_value *
data_base::find_with_time
(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name,
 db_value * found_value,
 const key& _key, uint64_t time_parameter) const
{
    this->trace_with_guard("data_base::find_with_time method started");
    if (found_value == nullptr) {
        associative_container<key, db_value *> * collection = find_data_collection(pool_name, scheme_name, collection_name);

        try {
            found_value = collection->get(_key);
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
            this->debug_with_guard("data_base::find_with_time cannot find a found_value with passed key")
                    ->trace_with_guard("data_base::find_with_time method finished");
            throw data_base::db_find_exception("find_with_time:: cannot find a found_value with passed key");
        }
    }

    db_value * found_value_copy = found_value->make_a_copy();
    handler * first_handler = found_value->get_first_handler();

    if (first_handler == nullptr) {
        return found_value_copy;
    }

    first_handler->handle(&found_value_copy, time_parameter);

    this->trace_with_guard("data_base::find_with_time method finished");
    return found_value_copy;
}

std::vector<db_value *>
data_base::find_in_range(const std::string &pool_name, const std::string &scheme_name,
                         const std::string &collection_name, key min_key, key max_key) const
{
    this->trace_with_guard("data_base::find_in_range method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);

    key_comparer comparer;
    auto comparison_result = comparer(min_key, max_key);
    if (comparison_result > 0) {
        key tmp = min_key;
        min_key = max_key;
        max_key = tmp;
    }

    std::vector<db_value *> to_return_vector;

    if (comparison_result == 0) {
        try {
            db_value * found = data_collection->get(min_key);
            to_return_vector.push_back(found);
            return to_return_vector;
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
            throw db_find_exception("data_base::find_in_range no appropriate values found in collection");
        }
    }

    // done only for bst-like trees. B and B+ trees require another iterators
    auto * bst = reinterpret_cast<bs_tree<key, db_value *, key_comparer> *>(data_collection);
    auto end_iteration = bst->end_infix();
    bool in_range = false;

    for (auto it = bst->begin_infix(); it != end_iteration; ++it) {
        if (in_range) {
            if (comparer(max_key, std::get<1>(*it)) >= 0) {
                to_return_vector.push_back(std::get<2>(*it));
            } else {
                break;
            }
        }

        if (!in_range) {
            if (comparer(min_key, std::get<1>(*it)) <= 0) {
                in_range = true;
                to_return_vector.push_back(std::get<2>(*it));
            }
        }
    }

    this->trace_with_guard("data_base::find_in_range method finished");
    return to_return_vector;
}

#pragma endregion

#pragma region Deletion from collection
void
data_base::delete_from_collection(const std::string &pool_name, const std::string &scheme_name,
                                  const std::string &collection_name, const key& _key) const
{
    this->trace_with_guard("data_base::delete_from_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pool_name, scheme_name, collection_name);

    try {
        db_value * found_value = data_collection->get(_key);
        handler * delete_handle = new remove_handler();
        handler * last_handler = found_value->get_last_handler();
        if (last_handler == nullptr) {
            found_value->add_new_handler(delete_handle);
        } else {
            if (last_handler->get_handler_type() != handler::handler_types::_remove_handler_) {
                found_value->add_new_handler(delete_handle);
            } else {
                this->warning_with_guard("data_base::delete_from_collection delete handler cannot be after delete handler in chain of responsibility")
                        ->trace_with_guard("data_base::delete_from_collection method finished");
                throw handler::order_exception("update_in_collection:: delete handler cannot be after delete handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::delete_from_collection no element with passed _key found")
                ->trace_with_guard("data_base::delete_from_collection method finished");
        throw data_base::db_find_exception("delete_from_collection:: no element with passed _key found");
    }
    this->trace_with_guard("data_base::delete_from_collection method finished");
}

#pragma endregion

#pragma endregion

#pragma region Structure functions
#pragma region Inserting in structure of data base

memory *
data_base::get_new_allocator_for_inner_trees
        (std::string const & pool_name, std::string const & scheme_name, std::string const &collection_name,
         data_base::allocator_types_ allocator_type, size_t allocator_pool_size)
{
    this->trace_with_guard("data_base::get_new_allocator_for_inner_trees method started");
    memory * new_allocator = nullptr;
    switch (allocator_type) {
        case data_base::allocator_types_ ::global:
            new_allocator = new memory_from_global_heap(this->get_logger());
            break;
        case data_base::allocator_types_ ::sorted_list_best:
            new_allocator = new memory_with_sorted_list_deallocation(allocator_pool_size, memory::Allocation_strategy::best_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::sorted_list_worst:
            new_allocator = new memory_with_sorted_list_deallocation(allocator_pool_size, memory::Allocation_strategy::worst_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::sorted_list_first:
            new_allocator = new memory_with_sorted_list_deallocation(allocator_pool_size, memory::Allocation_strategy::first_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::descriptors_best:
            new_allocator = new memory_with_boundary_tags(allocator_pool_size, memory::Allocation_strategy::best_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::descriptors_worst:
            new_allocator = new memory_with_boundary_tags(allocator_pool_size, memory::Allocation_strategy::worst_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::descriptors_first:
            new_allocator = new memory_with_boundary_tags(allocator_pool_size, memory::Allocation_strategy::first_fit, this->get_logger(), nullptr);
            break;
        case data_base::allocator_types_ ::buddy_system:
            new_allocator = new memory_with_buddy_system((char)log2((double)allocator_pool_size), this->get_logger(), nullptr);
            break;
        default:
            break;
    }
    if (new_allocator != nullptr) {
        _all_trees_allocators[pool_name] = new_allocator;
    }

    this->trace_with_guard("data_base::get_new_allocator_for_inner_trees method finished");
    return new_allocator;
}

void
data_base::add_to_structure(const std::string &pool_name, const std::string &scheme_name,
                            const std::string &collection_name,
                            data_base::trees_types_ tree_type,
                            data_base::allocator_types_ allocator_type,
                            size_t allocator_pool_size)
{
    this->trace_with_guard("data_base::add_to_structure method started");
    if (pool_name.empty()) {
        this->warning_with_guard("data_base::add_to_structure one should pass pool name for correct work of method")
                ->trace_with_guard("data_base::add_to_structure method finished");
        throw data_base::db_insert_exception("add_to_structure:: one should pass pool name for correct work of method");
    }

    // insert data pool in _database
    if (scheme_name.empty()) {
        memory * new_allocator = get_new_allocator_for_inner_trees(pool_name, scheme_name, collection_name, allocator_type, allocator_pool_size);

        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pool = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_pool = new bs_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_pool = new avl_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_pool = new rb_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_pool = new splay_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            _database->insert(pool_name, std::move(data_pool));
        }
        catch (bs_tree<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, associative_container<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, associative_container<key, db_value*>*>*, data_base::string_comparer>::insert_exception const &) {
            delete data_pool;
            _all_trees_allocators.erase(pool_name);
            delete new_allocator;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique pool name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: insert failed due to non-unique pool name to insert");
        }
        catch (typename bs_tree<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *, data_base::string_comparer>::insert_exception const &) {
            delete data_pool;
            _all_trees_allocators.erase(pool_name);
            delete new_allocator;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique pool name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: insert failed due to non-unique pool name to insert");
        }
        catch (bs_tree<std::string,
                associative_container<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *> *, data_base::string_comparer>::insert_exception const &) {
            delete data_pool;
            _all_trees_allocators.erase(pool_name);
            delete new_allocator;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique pool name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: insert failed due to non-unique pool name to insert");
        }
    }
        // insert data scheme
    else if (collection_name.empty()) {
        memory * new_allocator = nullptr;
        if (_all_trees_allocators.contains(pool_name)) {
            new_allocator = _all_trees_allocators[pool_name];
        }

        // find a pool
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pool;
        try {
            data_pool = find_data_pool(pool_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::add_to_structure no such pool name in data_base")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: no such pool name in data_base");
        }

        associative_container<std::string,
                associative_container<key, db_value *> *> * data_scheme = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_scheme = new bs_tree<std::string,
                                    associative_container<key, db_value *> *,
                                        string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_scheme = new avl_tree<std::string,
                        associative_container<key, db_value *> *,
                        string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_scheme = new rb_tree<std::string,
                        associative_container<key, db_value *> *,
                        string_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_scheme = new splay_tree<std::string,
                        associative_container<key, db_value *> *,
                        string_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            data_pool->insert(scheme_name, std::move(data_scheme));
        }
        catch (typename bs_tree<std::string,
                associative_container<key, db_value *>, string_comparer>::insert_exception const &) {
            delete data_scheme;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique scheme name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique scheme name to insert");
        }
    }
        // insert collection
    else {
        // find a scheme
        memory * new_allocator = nullptr;
        if (_all_trees_allocators.contains(pool_name)) {
            new_allocator = _all_trees_allocators[pool_name];
        }

        associative_container<std::string,
                associative_container<key, db_value *> *> * data_scheme;

        try {
            data_scheme = find_data_scheme(pool_name, scheme_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::add_to_structure no such pool/scheme name in data_base")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: no such pool/scheme name in data_base");
        }

        associative_container<key, db_value *> * data_collection = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_collection = new bs_tree<key, db_value *, key_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_collection = new avl_tree<key, db_value *, key_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_collection = new rb_tree<key, db_value *, key_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_collection = new splay_tree<key, db_value *, key_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            data_scheme->insert(collection_name, std::move(data_collection));
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::insert_exception const &) {
            delete data_collection;
            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique collection name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique collection name to insert");
        }
    }
    this->trace_with_guard("data_base::add_to_structure method finished");
}

#pragma endregion

#pragma region Deleting from structure of data base

void data_base::delete_pool(const std::string & pool_name)
{
    associative_container<std::string,
            associative_container<std::string,
                associative_container<key, db_value *> *> *> * data_pool = _database->remove(pool_name);

    delete data_pool;

    if (_all_trees_allocators.contains(pool_name)) {
        memory * tmp = _all_trees_allocators[pool_name];
        _all_trees_allocators.erase(pool_name);
        delete tmp;
    }
}

// full path must include pool/scheme/
void data_base::delete_scheme(const std::string & scheme_name, associative_container<std::string,
                              associative_container<std::string, associative_container<key, db_value *> *> *> * parent_pool)
{
    associative_container<std::string,
            associative_container<key, db_value *> *> * data_scheme = parent_pool->remove(scheme_name);

    delete data_scheme;
}

void data_base::delete_collection(const std::string & collection_name, associative_container<std::string,
        associative_container<key, db_value *> *> * parent_scheme)
{
    associative_container<key, db_value *> * data_collection = parent_scheme->remove(collection_name);

    delete data_collection;
}

void
data_base::delete_from_structure(const std::string &pool_name, const std::string &scheme_name,
                                 const std::string &collection_name)
{
    this->trace_with_guard("data_base::delete_from_structure method started");
    if (pool_name.empty()) {
        this->warning_with_guard("data_base::delete_from_structure one should pass pool name for correct work of method")
                ->trace_with_guard("data_base::delete_from_structure method finished");
        throw data_base::db_remove_exception(
                "delete_from_structure:: one should pass pool name for correct work of method");
    }

    // delete pool. should delete all schemes' collections' allocators, delete all schemes' allocators, delete pool's allocator
    if (scheme_name.empty()) {
        try {
            delete_pool(pool_name);
        }
        catch (bs_tree<std::string,
                    associative_container<std::string,
                        associative_container<std::string,
                            associative_container<key, db_value *> *> *> *, data_base::string_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure not found passed pool " + pool_name)
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw db_remove_exception("delete_from_structure:: not found passed pool " + pool_name);
        }
    }
    // deleting scheme
    else if (collection_name.empty()) {
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pool;
        try {
            data_pool = find_data_pool(pool_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no such pool name in data_base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception("delete_from_structure:: no such pool name in data_base");
        }

        try {
            delete_scheme(scheme_name, data_pool);
        }
        catch (typename bs_tree<std::string,
                            associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no scheme with name " + scheme_name + "in data base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no scheme with name " + scheme_name + " in data base");
        }
    }
    // deleting collection
    else {
        // find a scheme
        associative_container<std::string,
                associative_container<key, db_value *> *> * data_scheme;
        try {
            data_scheme = find_data_scheme(pool_name, scheme_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no such pool/scheme name in data_base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "add_to_structure:: no such pool/scheme name in data_base");
        }

        try {
            delete_collection(collection_name, data_scheme);
        }
        catch (typename bs_tree<std::string, associative_container<key, db_value *> *, string_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no collection with name " + collection_name + "in data base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no collection with name " + collection_name + " in data base");
        }
    }

    this->trace_with_guard("data_base::delete_from_structure method finished");
}

data_base::~data_base() {
    if (_database == nullptr) {
        this->warning_with_guard("data_base::~data_base data_base has already been removed");
        return;
    }
    //  done only for bst-like trees

    this->trace_with_guard("data_base::~data_base() method started");

    for (auto&& p : _all_trees_allocators) { delete p.second; }
    _all_trees_allocators.clear();

    delete _database;
    this->_database = nullptr;

    this->trace_with_guard("data_base::~data_base() method finished");
}

#pragma endregion
#pragma endregion
