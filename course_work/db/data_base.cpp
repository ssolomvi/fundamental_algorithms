#include "data_base.h"

#pragma region Find structure

associative_container<std::string, associative_container<std::string, associative_container<key, db_value *> *> *> *
data_base::find_data_pull(const std::string &pull_name)
{
    this->trace_with_guard("data_base::find_data_pull method started");
    if (pull_name.empty()) {
        this->warning_with_guard("data_base::find_data_pull pull name must not be an empty string")
                ->trace_with_guard("data_base::find_data_pull method finished");
        throw data_base::db_find_exception("find_data_pull:: pull name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *> *> *data_pull = nullptr;

    try {
        data_pull = this->_database->get(pull_name);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_data_pull data pull not found")
                ->trace_with_guard("data_base::find_data_pull method finished");
        throw data_base::db_find_exception("find_data_pull:: data pull not found");
    }

    this->trace_with_guard("data_base::find_data_pull method finished");
    return data_pull;
}

associative_container<std::string, associative_container<key, db_value *> *> *
data_base::find_data_scheme(const std::string &pull_name, const std::string &scheme_name) {
    this->trace_with_guard("data_base::find_data_scheme method started");
    if (scheme_name.empty()) {
        this->warning_with_guard("data_base::find_data_scheme scheme name must not be an empty string")
                ->trace_with_guard("data_base::find_data_scheme method finished");
        throw data_base::db_find_exception("find_data_scheme:: scheme name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *> *> *data_pull = find_data_pull(pull_name);

    associative_container<std::string,
            associative_container<key, db_value *> *> *data_scheme = nullptr;

    try {
        data_scheme = data_pull->get(scheme_name);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_data_scheme data scheme not found")
                ->trace_with_guard("data_base::find_data_scheme method finished");
        throw data_base::db_find_exception("find_data_scheme:: data scheme not found");
    }

    this->trace_with_guard("data_base::find_data_scheme method finished");
    return data_scheme;
}

associative_container<key, db_value *> *
data_base::find_data_collection(const std::string &pull_name,
                                const std::string &scheme_name,
                                const std::string &collection_name) {
    this->trace_with_guard("data_base::find_data_collection method finished");

    if (collection_name.empty()) {
        this->warning_with_guard("data_base::find_data_collection collection name must not be an empty string")
                ->trace_with_guard("data_base::find_data_collection method finished");
        throw data_base::db_find_exception("find_data_collection:: collection name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<key, db_value *> *> *data_scheme = find_data_scheme(pull_name, scheme_name);

    associative_container<key, db_value *> *data_collection = nullptr;

    try {
        data_collection = data_scheme->get(collection_name);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
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
data_base::add_to_collection(const std::string &pull_name,
                             const std::string &scheme_name,
                             const std::string &collection_name, key _key,
                             db_value * value) {
    this->trace_with_guard("data_base::add_to_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);
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
data_base::update_in_collection(const std::string &pull_name, const std::string &scheme_name,
                                const std::string &collection_name, key _key,
                                std::map<db_value_fields, unsigned char *> upd_dict)
{
    this->trace_with_guard("data_base::update_in_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);
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
data_base::find_among_collection(const std::string &pull_name, const std::string &scheme_name,
                                 const std::string &collection_name, key _key)
{
    this->trace_with_guard("data_base::find_among_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value = nullptr;
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

db_value *
data_base::find_with_time
        (const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
         key _key, uint64_t time_parameter)
{
    this->trace_with_guard("data_base::find_with_time method started");
    associative_container<key, db_value *> * collection = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value = nullptr;
    try {
        found_value = collection->get(_key);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        this->debug_with_guard("data_base::find_with_time cannot find a value with passed key")
                ->trace_with_guard("data_base::find_with_time method finished");
        throw data_base::db_find_exception("find_with_time:: cannot find a value with passed key");
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
data_base::find_in_range(const std::string &pull_name, const std::string &scheme_name,
                         const std::string &collection_name, key min_key, key max_key)
{
    this->trace_with_guard("data_base::find_in_range method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    key_comparer comparer;
    if (comparer(min_key, max_key) > 0) {
        key tmp = min_key;
        min_key = max_key;
        max_key = tmp;
    }
    // todo: if comparer(min_key, max_key) == 0, do simple finding

    std::vector<db_value *> to_return_vector;

    // done only for bst-like trees. B and B+ trees require another iterators
    auto * bst = reinterpret_cast<bs_tree<key, db_value *, key_comparer> *>(data_collection);
    auto end_iteration = bst->end_infix();
    bool in_range = false;

    for (auto it = bst->begin_infix(); it != end_iteration; ++it) {
        if (in_range) {
            if (comparer(max_key, std::get<1>(*it)) >= 0) {
                to_return_vector.push_back(std::get<2>(*it));
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
data_base::delete_from_collection(const std::string &pull_name, const std::string &scheme_name,
                                  const std::string &collection_name, key _key)
{
    this->trace_with_guard("data_base::delete_from_collection method started");
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value;
    try {
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
        (std::string const & pull_name, std::string const & scheme_name, std::string const &collection_name,
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
            new_allocator = new memory_with_buddy_system(log2(allocator_pool_size), this->get_logger(), nullptr);
            break;
        default:
            break;
    }
    if (new_allocator != nullptr) {
        _all_trees_allocators[pull_name + "/" + scheme_name + "/" + collection_name] = new_allocator;
    }

    this->trace_with_guard("data_base::get_new_allocator_for_inner_trees method finished");
    return new_allocator;
}

void
data_base::add_to_structure(const std::string &pull_name, const std::string &scheme_name,
                            const std::string &collection_name,
                            data_base::trees_types_ tree_type,
                            data_base::allocator_types_ allocator_type,
                            size_t allocator_pool_size)
{
    this->trace_with_guard("data_base::add_to_structure method started");
    if (pull_name.empty()) {
        this->warning_with_guard("data_base::add_to_structure one should pass pull name for correct work of method")
                ->trace_with_guard("data_base::add_to_structure method finished");
        throw data_base::db_insert_exception("add_to_structure:: one should pass pull name for correct work of method");
    }

    memory * new_allocator = get_new_allocator_for_inner_trees(pull_name, scheme_name, collection_name, allocator_type, allocator_pool_size);

    // insert data pool in _database
    if (scheme_name.empty()) {
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pull = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_pull = new bs_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_pull = new avl_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_pull = new rb_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_pull = new splay_tree<std::string,
                        associative_container<std::string,
                                associative_container<key, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            _database->insert(pull_name, std::move(data_pull));
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::insert_exception const &) {
            delete data_pull; // ?? todo: check
            _all_trees_allocators.erase(pull_name + "/" + scheme_name + "/" + collection_name);
            delete new_allocator;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique pull name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: insert failed due to non-unique pull name to insert");
        }
    }
        // insert data scheme
    else if (collection_name.empty()) {
        // find a pool
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pull = nullptr;
        try {
            data_pull = find_data_pull(pull_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::add_to_structure no such pull name in data_base")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: no such pull name in data_base");
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
            data_pull->insert(scheme_name, std::move(data_scheme));
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::insert_exception const &) {
            delete data_scheme; // ?? todo: check
            _all_trees_allocators.erase(pull_name + "/" + scheme_name + "/" + collection_name);
            delete new_allocator;

            this->warning_with_guard("data_base::add_to_structure insert failed due to non-unique scheme name to insert")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique scheme name to insert");
        }
    }
        // insert collection
    else {
        // find a scheme
        associative_container<std::string,
                associative_container<key, db_value *> *> * data_scheme = nullptr;

        try {
            data_scheme = find_data_scheme(pull_name, scheme_name);
        }
        catch (data_base::db_find_exception const &) {
            _all_trees_allocators.erase(pull_name + "/" + scheme_name + "/" + collection_name);
            delete new_allocator;

            this->debug_with_guard("data_base::add_to_structure no such pull/scheme name in data_base")
                    ->trace_with_guard("data_base::add_to_structure method finished");
            throw data_base::db_insert_exception("add_to_structure:: no such pull/scheme name in data_base");
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
            delete data_collection; // ?? todo: check
            _all_trees_allocators.erase(pull_name + "/" + scheme_name + "/" + collection_name);
            delete new_allocator;

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

void
data_base::delete_from_structure_inner
        (void * to_delete, std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name)
{
    this->trace_with_guard("data_base::delete_from_structure_inner method started");
    // todo: done for bst-like only
    if (scheme_name.empty()) {
        auto * pull_to_d =  reinterpret_cast<bs_tree<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *, string_comparer> *>(to_delete);

        auto iter_end = pull_to_d->end_infix();
        for (auto iter = pull_to_d->begin_infix(); iter != iter_end; ++iter) {
            delete_from_structure_inner(reinterpret_cast<void *>(std::get<2>(*iter)), pull_name, std::get<1>(*iter), "");
        }

        delete pull_to_d;

        if (_all_trees_allocators.contains(pull_name)) {
            memory * this_pool_allocator = _all_trees_allocators[pull_name];
            delete this_pool_allocator;
            _all_trees_allocators.erase(pull_name);
        }

    } else if (collection_name.empty()) {
        auto * scheme_to_d = reinterpret_cast<bs_tree<std::string,
                associative_container<key, db_value *> *, string_comparer> *>(to_delete);

        auto iter_end = scheme_to_d->end_infix();
        for (auto iter = scheme_to_d->begin_infix(); iter != iter_end; ++iter) {
            delete_from_structure_inner(reinterpret_cast<void *>(std::get<2>(*iter)), pull_name, scheme_name, std::get<1>(*iter));
        }

        delete scheme_to_d;

        std::string scheme_full_name = pull_name + "/" + scheme_name;
        if (_all_trees_allocators.contains(scheme_full_name)) {
            memory * this_pool_allocator = _all_trees_allocators[scheme_full_name];
            delete this_pool_allocator;
            _all_trees_allocators.erase(scheme_full_name);
        }

    } else {
        auto * collection_to_d = reinterpret_cast<unsigned char *>(to_delete);

        delete collection_to_d;

        std::string full_path_to_collection = pull_name + "/" + scheme_name + "/" + collection_name;
        if (_all_trees_allocators.contains(full_path_to_collection)) {
            memory * this_pool_allocator = _all_trees_allocators[full_path_to_collection];
            delete this_pool_allocator;
            _all_trees_allocators.erase(full_path_to_collection);
        }
    }
    this->debug_with_guard("data_base::delete_from_structure_inner method finished");
}

void
data_base::delete_from_structure(const std::string &pull_name, const std::string &scheme_name,
                                 const std::string &collection_name)
{
    this->trace_with_guard("data_base::delete_from_structure method started");
    if (pull_name.empty()) {
        this->warning_with_guard("data_base::delete_from_structure one should pass pull name for correct work of method")
                ->trace_with_guard("data_base::delete_from_structure method finished");
        throw data_base::db_remove_exception(
                "delete_from_structure:: one should pass pull name for correct work of method");
    }
    // delete pull. should delete all schemes' collections' allocators, delete all schemes' allocators, delete pull's allocator
    if (scheme_name.empty()) {
        // 1. get pool
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *
                > *
        > * pull = nullptr;

        try {
            pull = _database->get(pull_name);
        }
        catch (bs_tree<key, db_value *, key_comparer> const &) {
            this->debug_with_guard("data_base::delete_from_structure not found passed pool " + pull_name)
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw db_remove_exception("delete_from_structure:: not found passed pool " + pull_name);
        }

    }

    // deleting pull
    // make sure to delete all its schemes and in schemes all collections and in collection all values
    if (scheme_name.empty()) {
        try {
            _database->remove(pull_name);
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no pool with name " + pull_name + "in data base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no pool with name " + pull_name + "in data base");
        }
    }
        // deleting scheme
    else if (collection_name.empty()) {
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<key, db_value *> *> *> *data_pull = nullptr;
        try {
            data_pull = find_data_pull(pull_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no such pull name in data_base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception("delete_from_structure:: no such pull name in data_base");
        }

        try {
            data_pull->remove(scheme_name);
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no scheme with name " + scheme_name + "in data base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no scheme with name " + scheme_name + "in data base");
        }
    }
        // deleting collection
    else {
        // find a scheme
        associative_container<std::string,
                associative_container<key, db_value *> *> * data_scheme = nullptr;

        try {
            data_scheme = find_data_scheme(pull_name, scheme_name);
        }
        catch (data_base::db_find_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no such pull/scheme name in data_base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "add_to_structure:: no such pull/scheme name in data_base");
        }

        try {
            data_scheme->remove(collection_name);
            // delete collection's allocator
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
            this->debug_with_guard("data_base::delete_from_structure no collection with name " + collection_name + "in data base")
                    ->trace_with_guard("data_base::delete_from_structure method finished");
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no collection with name " + collection_name + "in data base");
        }
    }

    this->trace_with_guard("data_base::delete_from_structure method finished");
}

data_base::~data_base() {
    // todo: done only for bst-like trees
    this->trace_with_guard("data_base::~data_base() method started");
    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *
            > *
    > * pull = nullptr;
    std::string pull_name;

    auto iter_end = _database->end_infix();
    for (auto iter = _database->begin_infix(); iter != iter_end; ++iter) {
        pull_name = std::get<1>(*iter);
        pull = std::get<2>(*iter);

        this->delete_from_structure_inner(reinterpret_cast<void *>(pull), pull_name, "", "");
    }

    this->trace_with_guard("data_base::~data_base() method finished");
    delete _database;
}

#pragma endregion
#pragma endregion
