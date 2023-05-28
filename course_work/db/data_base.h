#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <tuple>
#include <vector>
#include <cstring>
#include <ctime>

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

template<
        typename tkey,
        typename tkey_comparer>
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
                associative_container<tkey, db_value *> *
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
    associative_container<std::string, associative_container<std::string, associative_container<tkey, db_value *> *> *> *
    find_data_pull
    (std::string const & pull_name);

    associative_container<std::string, associative_container<tkey, db_value *> *> *
    find_data_scheme
    (std::string const & pull_name, std::string const & scheme_name);

    associative_container<tkey, db_value *> *
    find_data_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection
public:
    void add_to_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key, db_value * value);

#pragma endregion

#pragma region Updating a collection value
public:
    void update_in_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key, std::map<db_value_fields, unsigned char *> upd_dict);
#pragma endregion

#pragma region Finding among collection
public:
    db_value * find_among_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key);

    db_value * find_with_time
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
    tkey key, uint64_t time_parameter);

     std::vector<db_value *> find_in_range
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey min_key, tkey max_key);
#pragma endregion

#pragma region Deletion from collection
public:
    void delete_from_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key);
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
    // deleting a collection: no string is empty
    // deleting a scheme: collection string is empty
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
                                            associative_container<tkey, db_value *> *> *> *
                                                                     , string_comparer>(this_db_logger, this_db_allocator);
    }

    ~data_base() override
    {
        // todo: done only for bst-like trees
        auto iter_end = _database->end_infix();
        for (auto iter = _database->begin_infix(); iter != iter_end; ++iter) {
            this->delete_from_structure_inner(reinterpret_cast<void *>(std::get<2>(*iter)), std::get<1>(*iter), "", "");
        }

        delete _database;
    }

    // copy constructor
    data_base(data_base<tkey, tkey_comparer> const &obj) = delete;

    // move constructor
    data_base(data_base<tkey, tkey_comparer> &&obj) noexcept = delete;

    // copy assignment (оператор присваивания)
    data_base &operator=(data_base<tkey, tkey_comparer> const &obj) = delete;

    // move assignment (оператор присваивания перемещением)
    data_base &operator=(data_base<tkey, tkey_comparer> &&obj) noexcept = delete;

#pragma endregion
};

#endif //DATA_BASE_H
