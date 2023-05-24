#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <tuple>
#include <utility>
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
        typename tvalue,
        typename tkey_comparer>
class data_base final :
        private memory_holder,
        private logger_holder
{
    // TODO: расставить const
    // todo: подключить деревья, аллокаторы
private:
    /* дерево пулов
     *     дерево схем
     *          дерево коллекций
     *               коллекция
     * */
    associative_container<std::string,
        associative_container<std::string,
            associative_container<std::string,
                associative_container<tkey, tvalue> *
                                 > *
                             > *
                         > * _database;
    logger * _logger;
    memory* _allocator;

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
    class db_key_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_key_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_value_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_value_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

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
    associative_container<std::string, associative_container<std::string, associative_container<tkey, tvalue> *> *> **
    find_data_pull
    (std::string const & pull_name);

    associative_container<std::string, associative_container<tkey, tvalue> *> **
    find_data_scheme
    (std::string const & pull_name, std::string const & scheme_name);

    associative_container<tkey, tvalue> **
    find_data_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection
public:
    void add_to_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key, tvalue &&value);

#pragma endregion

#pragma region Updating a collection value
public:
    // todo: redo
    void update_in_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tvalue &&value);
#pragma endregion

#pragma region Finding among collection
public:
    db_value const & find_among_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key);

    std::vector<db_value> find_in_range
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey min_key, tkey max_key);
#pragma endregion

#pragma region Deletion from collection
public:
    db_value delete_from_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     tkey key);
#pragma endregion
#pragma endregion

#pragma region Structure functions
#pragma region Inserting in structure of data base
    // adding a collection: no string is empty
    // adding a scheme: collection string is empty
public:
    void add_to_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     trees_types_ tree_type, allocator_types_ allocator_type);
#pragma endregion

#pragma region Deletion from structure of data base
    // deleting a collection: no string is empty
    // deleting a scheme: collection string is empty
public:
    void delete_from_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion

#pragma region Changing the structure of data base
public:
    void change_the_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     trees_types_ tree_type, allocator_types_ allocator_type);
#pragma endregion
#pragma endregion

#pragma region Save-upload to-from file
public:
    void save_to_file(std::string const & filename)
    {

    };

    void upload_from_file(std::string const & filename)
    {

    }
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
    // todo: make a constructor from file (pattern builder)
#pragma endregion
};

#endif //DATA_BASE_H