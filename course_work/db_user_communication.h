#ifndef DB_USER_COMMUNICATION_H
#define DB_USER_COMMUNICATION_H

#include "db_value/db_value.h"
#include "db_value/db_value_builder.h"

#include "db_key/key.h"

void help();

// добавление новой записи
// add
// <key, value>
// + path to collection

// чтение записи по ключу
// find_pair
// key
// + path to collection

// чтение набора данных в диапазоне [min ; max]
// find_pair data set
// min
// max
// + path to collection

// обновление данных для записи по ключу
// update
// <key, value>
// + path to collection

std::pair<key, std::map<db_value_fields, unsigned char *>> get_update_key_and_dictionary(std::stringstream* input_stream, bool is_cin);
void update(db_value * value_to_update_to, std::map<db_value_fields, unsigned char *> dict);
void add(db_value * value_to_add_to, db_value * new_value);
void remove(db_value * value_to_delete);
db_value * find_value_version_time(db_value * value, uint64_t time_parameter);

typedef struct time_str
{
    short YY, MM, DD;
    short hh, mm, ss;
} struct_time;

uint64_t parse_time_from_stream(std::stringstream * input_stream, bool is_cin);
uint64_t convert_time_str_to_ms(time_str data);

typedef enum commands {
    _add_,
    _find_,
    _update_,
    _delete_,
    _save_,
    _upload_,
    _help_,
    _exit_,
    _not_a_command_
} commands_;

commands_ get_command(std::string const & user_input);

data_base<key, key_comparer>::trees_types_ get_tree_type(std::string const & user_input);

data_base<key, key_comparer>::allocator_types_ get_allocator_type(std::string const & user_input);

class parse_exception final : public std::exception {
private:
    std::string _message;

public:
    explicit parse_exception(std::string message)
            : _message(std::move(message)) {

    }

    [[nodiscard]] char const *what() const noexcept override {
        return _message.c_str();
    }
};

// returns a command, a tree type / allocator type, path
std::tuple<
        commands_,
        data_base<key, key_comparer>::trees_types_,
        data_base<key, key_comparer>::allocator_types_,
        size_t,
        std::string>
parse_user_input(std::string const & user_input);

std::tuple<std::string, std::string, std::string> parse_path(std::string & input_string);

std::tuple<std::string, std::string, std::string> get_path_from_user_input();

void do_add_command(data_base<key, key_comparer>::trees_types_ tree_type,
                    data_base<key, key_comparer>::allocator_types_ allocator_type,
                    std::string const & path_inner,
                    data_base<key, key_comparer>* db);

db_value & do_find_command(data_base<key, key_comparer> * db);

void do_update_command(data_base<key, key_comparer> * db);

void delete_db(data_base<key, key_comparer> * db);

void do_delete_command(std::string const & path_inner, data_base<key, key_comparer> * db);

void do_save_command(std::string const & path_inner, data_base<key, key_comparer> * db);

void do_upload_command(std::string const & path_inner, data_base<key, key_comparer> * db);

#endif //DB_USER_COMMUNICATION_H
