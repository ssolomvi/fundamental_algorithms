#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <tuple>
#include <vector>
#include <cstring>
#include <ctime>

class data_base final
        {
    // TODO: расставить const
    // todo: пронаследоваться от logger_holder, memory_holder
    // todo: подключить деревья
public:
    typedef struct key {
        unsigned applicant_id;
        unsigned contest_id;
        tm * utc_time;
    } key_struct;

    typedef struct value {
        char * surname, *name, *patronymic;
        char * birthday;
        char * link_to_resume;
        unsigned hr_id;
        char * programming_language;
        unsigned task_count;
        unsigned solved_task_count;
        bool copying;
    } value_struct;

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
        explicit db_key_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_value_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_value_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_insert_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_find_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_remove_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };
#pragma endregion

#pragma region Insertion in collection
public:
    void add_to_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key_struct key, value_struct value);

#pragma endregion

#pragma region Finding among collection
public:
    value_struct const & find_among_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key_struct key);
#pragma endregion

#pragma region Deletion from collection
public:
    value_struct delete_from_collection
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name,
     key_struct key);
#pragma endregion

#pragma region Inserting in structure of data base
    // adding a collection: no string is empty
    // adding a scheme: collection string is empty
public:
    void add_to_structure
    (std::string const & pull_name, std::string const & scheme_name, std::string const & collection_name);
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

#pragma region Save-upload to-from file
public:
    void save_to_file(std::string const & filename);

    void upload_from_file(std::string const & filename);
#pragma endregion

#pragma region rool 5
    // todo: make a constructor from file (pattern builder)
#pragma endregion
};


#endif //DATA_BASE_H
