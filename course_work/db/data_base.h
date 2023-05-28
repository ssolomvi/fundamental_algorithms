#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <tuple>
#include <vector>
#include <cstring>
#include <ctime>
#include <fstream>

#include "../allocator/memory_holder.h"
#include "../allocator_from_global_heap/memory_from_global_heap.h"
#include "../allocator_with_sorted_list_deallocation/memory_with_sorted_list_deallocation.h"
#include "../allocator_with_boundary_tags_deallocation/memory_with_boundary_tags.h"
#include "../allocator_with_buddy_system/memory_with_buddy_system.h"

#include "../logger/logger_holder.h"

#include "../binary_tree/associative_container.h"
#include "../binary_tree/bs_tree.h"
#include "../avl_tree/avl_tree.h"
#include "../splay_tree/splay_tree.h"
#include "../rb_tree/rb_tree.h"

//#include "../db_value/db_value.h"
#include "../chain_of_resp_and_command/handler.h"
#include "../db_key/key.h"

class data_base final :
        private memory_holder,
        private logger_holder
{
    // TODO: расставить const
    // todo: подключить аллокаторы
private:
    class string_comparer
    {
    public:
        int operator()(std::string const & x, std::string const & y) {
            return x.compare(y);
        }
    };

private:
    /* дерево пулов
     *     дерево схем
     *          дерево коллекций
     *               коллекция
     * */
    splay_tree<std::string,
        associative_container<std::string,
            associative_container<std::string,
                associative_container<key, db_value *> *
                                 > *
                             > *, string_comparer
                         > * _database;
    logger * _logger;
    memory* _allocator;
    std::map<std::string, memory *> _all_trees_allocators;

public:
    typedef enum trees_types {
        BST,
        AVL,
        SPLAY,
        RB,
        not_a_tree
    } trees_types_;

    typedef enum allocator_types {
        global,
        for_inner_use_sorted_list,
        sorted_list_best,
        sorted_list_worst,
        sorted_list_first,
        for_inner_use_descriptors,
        descriptors_best,
        descriptors_worst,
        descriptors_first,
        buddy_system,
        not_an_allocator
    } allocator_types_;

#pragma region exceptions
public:
    class db_insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_insert_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_find_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_remove_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };
#pragma endregion

#pragma region Find structure
private:
    associative_container<std::string, associative_container<std::string, associative_container<key, db_value *> *> *> *
    find_data_pull
    (std::string const & pull_name);

    associative_container<std::string, associative_container<key, db_value *> *> *
    find_data_scheme
    (std::string const & pull_name, std::string const & scheme_name);

    associative_container<key, db_value *> *
    find_data_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection
public:
    void add_to_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key _key, db_value * value);

#pragma endregion

#pragma region Updating a collection value
public:
    void update_in_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key _key, std::map<db_value_fields, unsigned char *> upd_dict);
#pragma endregion

#pragma region Finding among collection
public:
    db_value * find_among_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key _key);

    db_value * find_with_time
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key _key, uint64_t time_parameter);

     std::vector<db_value *> find_in_range
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key min_key, key max_key);
#pragma endregion

#pragma region Deletion from collection
public:
    void delete_from_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key key);
#pragma endregion
#pragma endregion

#pragma region Structure functions
#pragma region Inserting in structure of data base
    // adding a collection: no string is empty
    // adding a scheme: collection string is empty
private:
    memory *
    get_new_allocator_for_inner_trees
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     allocator_types_ allocator_type, size_t allocator_pool_size);

public:
    void add_to_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     trees_types_ tree_type, allocator_types_ allocator_type, size_t allocator_pool_size);
#pragma endregion

#pragma region Deletion from structure of data base
private:
    void delete_from_structure_inner
    (void * to_delete, std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);

public:
    void delete_from_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion
#pragma endregion

#pragma region Save-upload to-from file
public:
    /*
    // todo: make a constructor from file (pattern builder)
    void save_to_file(std::string const & filename)
    {

    };

    void upload_from_file(std::string const & filename)
    {

    }*/
#pragma endregion

#pragma region logger_holder and memory_holder contract
    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return this->_logger;
    }

    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return this->_allocator;
    }
#pragma endregion

#pragma region rool 5
public:
    explicit data_base(logger * this_db_logger = nullptr, memory * this_db_allocator = nullptr)
    : _logger(this_db_logger), _allocator(this_db_allocator)
    {
        _database = new splay_tree<std::string,
                                    associative_container<std::string,
                                        associative_container<std::string,
                                            associative_container<key, db_value *> *> *> *
                                                                     , string_comparer>(this_db_logger, this_db_allocator);
    }

    ~data_base() override;

    // copy constructor
    data_base(data_base const &obj) = delete;

    // move constructor
    data_base(data_base &&obj) noexcept = delete;

    // copy assignment (оператор присваивания)
    data_base &operator=(data_base const &obj) = delete;

    // move assignment (оператор присваивания перемещением)
    data_base &operator=(data_base &&obj) noexcept = delete;

#pragma endregion
};


#pragma region Find structure

associative_container<std::string, associative_container<std::string, associative_container<key, db_value *> *> *> *
data_base::find_data_pull(const std::string &pull_name) {
    if (pull_name.empty()) {
        throw data_base::db_find_exception("find_data_pull:: pull name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<std::string,
                    associative_container<key, db_value *> *> *> *data_pull = nullptr;

    try {
        data_pull = _database->get(pull_name);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception("find_data_pull:: data pull not found");
    }

    return data_pull;
}

associative_container<std::string, associative_container<key, db_value *> *> *
data_base::find_data_scheme(const std::string &pull_name, const std::string &scheme_name) {
    if (scheme_name.empty()) {
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
        throw data_base::db_find_exception("find_data_scheme:: data scheme not found");
    }

    return data_scheme;
}

associative_container<key, db_value *> *
data_base::find_data_collection(const std::string &pull_name,
                                                     const std::string &scheme_name,
                                                     const std::string &collection_name) {
    if (collection_name.empty()) {
        throw data_base::db_find_exception("find_data_collection:: collection name must not be an empty string");
    }

    associative_container<std::string,
            associative_container<key, db_value *> *> *data_scheme = find_data_scheme(pull_name, scheme_name);

    associative_container<key, db_value *> *data_collection = nullptr;

    try {
        data_collection = data_scheme->get(collection_name);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception("find_data_collection:: data collection not found");
    }

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
            throw handler::order_exception("add_to_collection:: add handler cannot be the first one in chain of responsibility");
        } else {
            // add command may be only after remove command
            if (last_handler->get_handler_type() == handler::handler_types::_remove_handler_) {
                found_value->add_new_handler(add_handle);
            } else {
                throw handler::order_exception("Add handler can be only after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        data_collection->insert(_key, std::move(value));
    }
}

#pragma endregion

#pragma region Updating a collection value
void
data_base::update_in_collection(const std::string &pull_name, const std::string &scheme_name,
                                                     const std::string &collection_name, key _key,
                                                     std::map<db_value_fields, unsigned char *> upd_dict)
{
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
                throw handler::order_exception("update_in_collection:: update handler cannot be after remove_handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception("update_in_collection:: no element with passed _key found");
    }
}

#pragma endregion

#pragma region Finding among collection
db_value *
data_base::find_among_collection(const std::string &pull_name, const std::string &scheme_name,
                                                      const std::string &collection_name, key _key)
{
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value = nullptr;
    try {
        found_value = data_collection->get(_key);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception("find_among_collection:: no value with passed _key found");
    }
    return found_value;
}

db_value *
data_base::find_with_time
        (const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
         key _key, uint64_t time_parameter)
{
    associative_container<key, db_value *> * collection = find_data_collection(pull_name, scheme_name, collection_name);

    db_value * found_value = nullptr;
    try {
        found_value = collection->get(_key);
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception("find_with_time:: cannot find a value with passed _key");
    }

    db_value * found_value_copy = found_value->make_a_copy();
    handler * first_handler = found_value->get_first_handler();

    if (first_handler == nullptr) {
        return found_value_copy;
    }

    first_handler->handle(&found_value_copy, time_parameter);
    return found_value_copy;
}

std::vector<db_value *>
data_base::find_in_range(const std::string &pull_name, const std::string &scheme_name,
                                              const std::string &collection_name, key min_key, key max_key)
{
    associative_container<key, db_value *> *data_collection
            = find_data_collection(pull_name, scheme_name, collection_name);

    key_comparer comparer;
    if (comparer(min_key, max_key) > 0) {
        key tmp = min_key;
        min_key = max_key;
        max_key = tmp;
    }

    std::vector<db_value *> to_return_vector;

    // todo: done only for bst-like trees. B and B+ trees require another iterators
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

    return to_return_vector;
}

#pragma endregion

#pragma region Deletion from collection
void
data_base::delete_from_collection(const std::string &pull_name, const std::string &scheme_name,
                                                       const std::string &collection_name, key _key)
{
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
                throw handler::order_exception("update_in_collection:: delete handler cannot be after delete handler in chain of responsibility");
            }
        }
    }
    catch (typename bs_tree<key, db_value *, key_comparer>::find_exception const &) {
        throw data_base::db_find_exception(
                "delete_from_collection:: no element with passed _key found");
    }
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

    return new_allocator;
}

void
data_base::add_to_structure(const std::string &pull_name, const std::string &scheme_name,
                                                 const std::string &collection_name,
                                                 data_base::trees_types_ tree_type,
                                                 data_base::allocator_types_ allocator_type,
                                                 size_t allocator_pool_size)
{
    if (pull_name.empty()) {
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
            throw data_base::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique pull name to insert");
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
            throw data_base::db_insert_exception(
                    "add_to_structure:: insert failed due to non-unique collection name to insert");
        }
    }
}

#pragma endregion

#pragma region Deleting from structure of data base

void
data_base::delete_from_structure_inner
        (void * to_delete, std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name)
{
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
}

void
data_base::delete_from_structure(const std::string &pull_name, const std::string &scheme_name,
                                                      const std::string &collection_name)
{
    if (pull_name.empty()) {
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
            throw db_remove_exception("delete_from_structure:: not found passed pool " + pull_name);
        }

    }

    // deleting pull
    // make sure to delete all its schemes and in schemes all collections and in collection all values
    if (scheme_name.empty()) {
        try {
            _database->remove(pull_name);
            // todo: delete all schemes' collections' allocators, delete all schemes' allocators, delete pull's allocator
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
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
            throw data_base::db_remove_exception("delete_from_structure:: no such pull name in data_base");
        }

        try {
            data_pull->remove(scheme_name);
            // todo: delete all scheme's collections' allocators, delete scheme's allocator
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
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
            throw data_base::db_remove_exception(
                    "add_to_structure:: no such pull/scheme name in data_base");
        }

        try {
            data_scheme->remove(collection_name);
            // delete collection's allocator
        }
        catch (typename bs_tree<key, db_value *, key_comparer>::remove_exception const &) {
            throw data_base::db_remove_exception(
                    "delete_from_structure:: no collection with name " + collection_name + "in data base");
        }
    }
}

data_base::~data_base() {
    // todo: done only for bst-like trees
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

    delete _database;
}

#pragma endregion
#pragma endregion

#endif //DATA_BASE_H
