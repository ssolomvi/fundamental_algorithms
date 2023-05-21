#ifndef DB_USER_COMMUNICATION_H
#define DB_USER_COMMUNICATION_H

void help();

// добавление новой записи
// add
// <key, value>
// + path to collection

// чтение записи по ключу
// find
// key
// + path to collection

// чтение набора данных в диапазоне [min ; max]
// find data set
// min
// max
// + path to collection

// обновление данных для записи по ключу
// update
// <key, value>
// + path to collection

typedef enum commands {
    _add_,
    _find_,
    _update_,
    _delete_,
    _change_,
    _save_,
    _upload_,
    _help_,
    _exit_,
    _not_a_command_
} commands_;

commands_ get_command(std::string const & user_input);

data_base::trees_types_ get_tree_type(std::string const & user_input);

data_base::allocator_types_ get_allocator_type(std::string const & user_input);

// returns a command, a tree type / allocator type, path
std::tuple<commands_, data_base::trees_types_, data_base::allocator_types_, std::string const &> parse_user_input(std::string const & user_input);

data_base::key_struct get_key_from_user_input();

data_base::value_struct get_value_from_user_input();

std::tuple<std::string, std::string, std::string> parse_path(std::string & input_string);

std::tuple<std::string, std::string, std::string> get_path_from_user_input();

void do_add_command(time_t now, std::string const & path_inner, data_base* db);

void do_find_command(data_base * db);

void do_update_command(data_base * db);

void delete_db(data_base * db);

void do_delete_command(std::string const & path_inner, data_base * db);

void do_change_command(data_base::trees_types_ tree_type, data_base::allocator_types_ allocator_type, std::string const & path_inner, data_base * db);

void do_save_command(std::string const & path_inner, data_base * db);

void do_upload_command(std::string const & path_inner, data_base * db);

#endif //DB_USER_COMMUNICATION_H
