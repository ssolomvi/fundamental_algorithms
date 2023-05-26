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
void
data_base<tkey, tkey_comparer>::add_to_structure(const std::string &pull_name, const std::string &scheme_name,
                                                 const std::string &collection_name,
                                                 data_base::trees_types_ tree_type,
                                                 data_base::allocator_types_ allocator_type)
{
    
}

template<typename tkey, typename tkey_comparer>
void
data_base<tkey, tkey_comparer>::delete_from_structure(const std::string &pull_name, const std::string &scheme_name,
                                                      const std::string &collection_name)
{

}


#pragma endregion
#pragma endregion