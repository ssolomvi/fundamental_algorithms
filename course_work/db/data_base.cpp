#include "data_base.h"
/*
template<typename key_comparator>
associative_container<std::string,
    associative_container<std::string,
        associative_container<key, db_value> *
        > *
    > **
data_base<key_comparator>::find_data_pull
(const std::string &pull_name)
{
    return nullptr;
}

template<typename key_comparator>
associative_container<std::string,
    associative_container<typename data_base<key_comparator>::key, db_value> *
            > **
data_base<key_comparator>::find_data_scheme
(const std::string &pull_name, const std::string &scheme_name)
{
    return nullptr;
}

template<typename key_comparator>
associative_container<typename data_base<key_comparator>::key, db_value> **
data_base<key_comparator>::find_data_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name)
{
    return nullptr;
}

template<typename key_comparator>
void
data_base<key_comparator>::add_to_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key, db_value &&value)
{

}

template<typename key_comparator>
void
data_base<key_comparator>::update_in_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key, data_base::key_struct new_key, db_value &&value)
{

}

template<typename key_comparator>
db_value const &
data_base<key_comparator>::find_among_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key)
{
//    return <#initializer#>;
}

template<typename key_comparator>
std::vector<db_value>
data_base<key_comparator>::find_in_range
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct min_key, data_base::key_struct max_key)
{
    return std::vector<db_value>();
}

template<typename key_comparator>
db_value
data_base<key_comparator>::delete_from_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key)
{
//    return db_value(__cxx11::basic_string(), __cxx11::basic_string(), __cxx11::basic_string(), __cxx11::basic_string(),
//                    __cxx11::basic_string(), 0, __cxx11::basic_string(), 0, 0, false);
}

*/

#pragma region Find structure

template<typename tkey, typename tkey_comparer>
associative_container<std::string, associative_container<std::string, associative_container<tkey, db_value *> *> *> *
data_base<tkey, tkey_comparer>::find_data_pull(const std::string &pull_name) {
    associative_container<std::string,
            associative_container<std::string,
                    associative_container<tkey, db_value *> *> *> *data_pull = nullptr;

    try {
        data_pull = _database->get(pull_name);
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception("find_data_pull:: data pull not found");
    }

    return data_pull;
}

template<typename tkey, typename tkey_comparer>
associative_container<std::string, associative_container<tkey, db_value *> *> *
data_base<tkey, tkey_comparer>::find_data_scheme(const std::string &pull_name, const std::string &scheme_name) {
    associative_container<std::string,
            associative_container<std::string,
                    associative_container<tkey, db_value *> *> *> *data_pull = find_data_pull(pull_name);

    associative_container<std::string,
            associative_container<tkey, db_value *> *> *data_scheme = nullptr;

    try {
        data_scheme = data_pull->get(scheme_name);
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception("find_data_scheme:: data scheme not found");
    }

    return data_scheme;
}

template<typename tkey, typename tkey_comparer>
associative_container<tkey, db_value *> *
data_base<tkey, tkey_comparer>::find_data_collection(const std::string &pull_name,
                                                     const std::string &scheme_name,
                                                     const std::string &collection_name) {
    associative_container<std::string,
            associative_container<tkey, db_value *> *> *data_scheme = find_data_scheme(pull_name, scheme_name);

    associative_container<tkey, db_value *> *data_collection = nullptr;

    try {
        data_collection = data_scheme->get(collection_name);
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception("find_data_collection:: data collection not found");
    }

    return data_collection;
}

#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection

template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::add_to_collection(const std::string &pull_name,
                                                              const std::string &scheme_name,
                                                              const std::string &collection_name, tkey key,
                                                              db_value * value) {
    associative_container<tkey, db_value *> *data_collection
        = find_data_collection(pull_name, scheme_name, collection_name);
    // handle if not found ?
    db_value * found_value;
    try {
        found_value = data_collection->get(key);
        handler *add_handle = new add_handler(value);
        handler **last_handler = found_value->get_last_handler();
        if ((*last_handler) == nullptr) {
            // there are no commands added
            throw handler::order_exception("Add handler cannot be the first one in chain of responsibility");
        } else {
            // add command may be only after remove command
            if ((*last_handler)->get_handler_type() == handler::handler_types::_remove_handler_) {
                (*last_handler)->set_next(add_handle);
            } else {
                throw handler::order_exception("Add handler can be only after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        data_collection->insert(key, value);
    }
}

#pragma endregion

#pragma region Updating a collection value
template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::update_in_collection(const std::string &pull_name, const std::string &scheme_name,
                                                     const std::string &collection_name, tkey key,
                                                     std::map<db_value_fields, unsigned char *> upd_dict)
{
    associative_container<tkey, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);
    // handle if not found ?
    db_value * found_value;
    try {
        found_value = data_collection->get(key);
        handler * update_handle = new update_handler(std::move(upd_dict));
        handler ** last_handler = found_value->get_last_handler();
        if ((*last_handler) == nullptr) {
            (*last_handler) = update_handle;
        } else {
            if ((*last_handler)->get_handler_type() != handler::handler_types::_remove_handler_) {
                ((*last_handler)->set_next(update_handle));
            } else {
                throw handler::order_exception("update_in_collection:: update handler cannot be after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception("update_in_collection:: no element with passed key found");
    }
}

#pragma endregion

#pragma region Finding among collection
template<typename tkey, typename tkey_comparer>
db_value * const
data_base<tkey, tkey_comparer>::find_among_collection(const std::string &pull_name, const std::string &scheme_name,
                                                      const std::string &collection_name, tkey key)
{
    associative_container<tkey, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value = nullptr;
    try {
        found_value = data_collection->get(key);
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception("find_among_collection:: no value with passed key found");
    }
    return found_value;
}

template<typename tkey, typename tkey_comparer>
std::vector<db_value *>
data_base<tkey, tkey_comparer>::find_in_range(const std::string &pull_name, const std::string &scheme_name,
                                              const std::string &collection_name, tkey min_key, tkey max_key)
{
    associative_container<tkey, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);
//    return std::vector<db_value *>();
}

#pragma endregion

#pragma region Deletion from collection
template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::delete_from_collection(const std::string &pull_name, const std::string &scheme_name,
                                                       const std::string &collection_name, tkey key)
{
    associative_container<tkey, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value;
    try {
        found_value = data_collection->get(key);
        handler * delete_handle = new remove_handler();
        handler ** last_handler = found_value->get_last_handler();
        if ((*last_handler) == nullptr) {
            (*last_handler) = delete_handle;
        } else {
            if ((*last_handler)) {
                ((*last_handler)->set_next(delete_handle));
            }
        }
    }
    catch (typename bs_tree<tkey, db_value *, tkey_comparer>::find_exception const &) {
        throw data_base<tkey, tkey_comparer>::db_find_exception(
                "delete_from_collection:: no element with passed key found");
    }
}

#pragma endregion

#pragma endregion

#pragma region Structure functions
#pragma region Inserting in structure of data base

template<typename tkey, typename tkey_comparer>
memory *
data_base<tkey, tkey_comparer>::get_new_allocator_for_inner_trees
(data_base<tkey, tkey_comparer>::allocator_types_ allocator_type, size_t allocator_pool_size)
{
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
    return new_allocator;
}

template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::add_to_structure(const std::string &pull_name, const std::string &scheme_name,
                                                 const std::string &collection_name,
                                                 data_base<tkey, tkey_comparer>::trees_types_ tree_type,
                                                 data_base<tkey, tkey_comparer>::allocator_types_ allocator_type,
                                                 size_t allocator_pool_size)
{
    if (pull_name.empty()) {
        throw data_base<tkey, tkey_comparer>::db_insert_exception("add_to_structure:: one should pass pull name for correct work of method");
    }

    memory * new_allocator = get_new_allocator_for_inner_trees(allocator_type, allocator_pool_size);

    // insert data pool in _database
    if (scheme_name.empty()) {
        associative_container<std::string,
                associative_container<std::string,
                    associative_container<tkey, db_value *> *> *> *data_pull = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_pull = new bs_tree<std::string,
                    associative_container<std::string,
                        associative_container<tkey, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_pull = new avl_tree<std::string,
                        associative_container<std::string,
                                associative_container<tkey, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_pull = new rb_tree<std::string,
                        associative_container<std::string,
                                associative_container<tkey, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_pull = new splay_tree<std::string,
                        associative_container<std::string,
                                associative_container<tkey, db_value *> *> *, string_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            _database->insert(pull_name, std::move(data_pull));
        }
        catch (typename bs_tree<tkey, db_value *, tkey_comparer>::insert_exception const &) {
            delete data_pull; // ?? todo: check
            throw data_base<tkey, tkey_comparer>::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique pull name to insert");
        }
    }
    // insert data scheme
    else if (collection_name.empty()) {
        // find a pool
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<tkey, db_value *> *> *> *data_pull = nullptr;
        try {
            data_pull = find_data_pull(pull_name);
        }
        catch (data_base<tkey, tkey_comparer>::db_find_exception const &) {
            throw data_base<tkey, tkey_comparer>::db_insert_exception("add_to_structure:: no such pull name in data_base");
        }

        associative_container<std::string,
            associative_container<tkey, db_value *> *> * data_scheme = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_scheme = new bs_tree<std::string,
                                associative_container<tkey, db_value *> *,
                                        string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_scheme = new avl_tree<std::string,
                        associative_container<tkey, db_value *> *,
                            string_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_scheme = new rb_tree<std::string,
                        associative_container<tkey, db_value *> *,
                            string_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_scheme = new splay_tree<std::string,
                        associative_container<tkey, db_value *> *,
                            string_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            data_pull->insert(scheme_name, std::move(data_scheme));
        }
        catch (typename bs_tree<tkey, db_value *, tkey_comparer>::insert_exception const &) {
            delete data_scheme; // ?? todo: check
            throw data_base<tkey, tkey_comparer>::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique scheme name to insert");
        }
    }
    // insert collection
    else {
        // find a scheme
        associative_container<std::string,
                associative_container<tkey, db_value *> *> * data_scheme = nullptr;

        try {
            data_scheme = find_data_scheme(pull_name, scheme_name);
        }
        catch (data_base<tkey, tkey_comparer>::db_find_exception const &) {
            throw data_base<tkey, tkey_comparer>::db_insert_exception("add_to_structure:: no such pull/scheme name in data_base");
        }

        associative_container<tkey, db_value *> * data_collection = nullptr;

        switch (tree_type) {
            case data_base::trees_types_::BST:
                data_collection = new bs_tree<tkey, db_value *, tkey_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::AVL:
                data_collection = new avl_tree<tkey, db_value *, tkey_comparer>(this->get_logger(), new_allocator);
                break;
            case data_base::trees_types_::RB:
                data_collection = new rb_tree<tkey, db_value *, tkey_comparer>(this->get_logger(), new_allocator);
                break;
            default:
                data_collection = new splay_tree<tkey, db_value *, tkey_comparer>(this->get_logger(), new_allocator);
                break;
        }

        try {
            data_scheme->insert(collection_name, std::move(data_collection));
        }
        catch (typename bs_tree<tkey, db_value *, tkey_comparer>::insert_exception const &) {
            delete data_collection; // ?? todo: check
            throw data_base<tkey, tkey_comparer>::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique collection name to insert");
        }
    }
}

template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::delete_from_structure(const std::string &pull_name, const std::string &scheme_name,
                                                      const std::string &collection_name)
{
    if (pull_name.empty()) {
        throw data_base<tkey, tkey_comparer>::db_insert_exception(
                "add_to_structure:: one should pass pull name for correct work of method");
    }

    // deleting pull
    // make sure to delete all its schemes and in schemes all collections and in collection all values
    if (scheme_name.empty()) {
        try {
            _database->remove(pull_name);
        }
        catch (typename bs_tree<tkey, db_value *, tkey_comparer>::remove_exception const &) {
            throw data_base<tkey, tkey_comparer>::db_remove_exception(
                    "delete_from_structure:: no pool with name " + pull_name + "in data base");
        }
    }
    // deleting scheme
    else if (collection_name.empty()) {
        associative_container<std::string,
                associative_container<std::string,
                        associative_container<tkey, db_value *> *> *> *data_pull = nullptr;
        try {
            data_pull = find_data_pull(pull_name);
        }
        catch (data_base<tkey, tkey_comparer>::db_find_exception const &) {
            throw data_base<tkey, tkey_comparer>::db_insert_exception(
                    "delete_from_structure:: no such pull name in data_base");
        }

        try {
            data_pull->remove(scheme_name);
        }
        catch (typename bs_tree<tkey, db_value *, tkey_comparer>::remove_exception const &) {
            throw data_base<tkey, tkey_comparer>::db_remove_exception(
                    "delete_from_structure:: no pool with name " + pull_name + "in data base");
        }
    }
    // deleting collection
    else {

    }
}


#pragma endregion
#pragma endregion