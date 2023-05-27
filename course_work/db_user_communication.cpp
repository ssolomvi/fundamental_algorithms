#include "db/data_base.h"
#include "db_user_communication.h"

void help() {
    std::cout << "----- Course work help -----" << std::endl;
    std::cout << "Collection commands list:" << std::endl;
    std::cout << "Full path to collection: <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- add" << std::endl;
    std::cout << "\t- find" << std::endl;
    std::cout << "\t- find dataset" << std::endl;
    std::cout << "\t- find DD/MM/YYYY hh/mm/ss" << std::endl;
    std::cout << "\t- update" << std::endl;
    std::cout << "\t- delete" << std::endl;
    std::cout << "Structural and customization commands list:" << std::endl;
    std::cout << "Supported trees: BST (binary tree), AVL, SPLAY, RB (red-black tree)" << std::endl;
    std::cout
            << "Supported allocators: global, sorted_list best||worst||first, descriptors best||worst||first, buddy_system"
            << std::endl;
    std::cout << "\t- add <tree type> <allocator type> <size of allocator> <full path>" << std::endl;
    std::cout << "\t\tone can note tree/allocator type only" << std::endl;
    std::cout
            << "\t\tif one wants to note an allocator, it's needed to also node desired length in bytes (not for global one)"
            << std::endl;
    std::cout << "\t- delete <full path>" << std::endl;
    std::cout << "DB commands list:" << std::endl;
    std::cout << "\t- save <filename>" << std::endl;
    std::cout << "\t- upload <filename>" << std::endl;
    std::cout << "\t- help" << std::endl;
    std::cout << "\t- delete DB" << std::endl;
    std::cout << "\t- exit" << std::endl;
}

#pragma region Parsing
commands_ get_command(std::string const &user_input) {
    if (user_input == "add") {
        return commands_::_add_;
    } else if (user_input == "find_pair") {
        return commands_::_find_;
    } else if (user_input == "update") {
        return commands_::_update_;
    } else if (user_input == "delete" || user_input == "remove") {
        return commands_::_delete_;
    } else if (user_input == "save") {
        return commands_::_save_;
    } else if (user_input == "upload") {
        return commands_::_upload_;
    } else if (user_input == "help") {
        return commands_::_help_;
    } else if (user_input == "exit") {
        return commands_::_exit_;
    }
    return commands_::_not_a_command_;
}

// returns a command, non-command info
std::pair<commands_, std::string>
parse_user_input(std::string const &user_input) {
    commands_ command = commands_::_not_a_command_;
    std::string token, s = user_input;
    size_t pos;

    // finding a command
    if ((pos = s.find(' ')) != std::string::npos) {
        command = get_command(s.substr(0, pos));
        s.erase(0, pos + 1);
    } else {
        s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
        command = get_command(user_input);
    }

    // a path is what is left from s
    return {command, s};
}

std::tuple<std::string, std::string, std::string>
parse_path(std::string &input_string) {
    input_string.erase(remove_if(input_string.begin(), input_string.end(), isspace), input_string.end());
    if (input_string.empty()) {
        throw parse_exception("parse_path:: incorrect path passed (is empty)");
    }

    std::string pull_name, scheme_name, collection_name;
    std::string delimiter = "/";
    unsigned delimiter_length = delimiter.size();
    size_t pos;

    if ((pos = input_string.find(delimiter)) != std::string::npos) {
        pull_name = input_string.substr(0, pos);
        input_string.erase(0, pos + delimiter_length);

        if ((pos = input_string.find(delimiter)) != std::string::npos) {
            scheme_name = input_string.substr(0, pos);
            input_string.erase(0, pos + delimiter_length);

            collection_name = input_string;
        } else {
            scheme_name = input_string;
        }
    } else {
        pull_name = input_string;
    }
    return {pull_name, scheme_name, collection_name};
}

// returns pull/scheme/collection
std::tuple<std::string, std::string, std::string>
get_path_from_user_input(std::ifstream *input_stream, bool is_cin, bool is_path) {
    std::string path_inner;

    if (is_cin) {
        (is_path
         ? std::cout << "Enter full path to collection: >>"
         : std::cout << "Enter full path to filename: >>");
    }

    (is_cin
     ? std::getline(std::cin, path_inner)
     : std::getline((*input_stream), path_inner));

    return parse_path(path_inner);
}

#pragma endregion

#pragma region Add command

data_base<key, key_comparer>::trees_types_ get_tree_type(std::string const &user_input) {
    if (user_input == "BST" || user_input == "bst") {
        return data_base<key, key_comparer>::trees_types_::BST;
    } else if (user_input == "AVL" || user_input == "avl") {
        return data_base<key, key_comparer>::trees_types_::AVL;
    } else if (user_input == "SPLAY" || user_input == "splay") {
        return data_base<key, key_comparer>::trees_types_::SPLAY;
    } else if (user_input == "RB" || user_input == "rb") {
        return data_base<key, key_comparer>::trees_types_::RB;
    }
    return data_base<key, key_comparer>::trees_types_::not_a_tree;
}

data_base<key, key_comparer>::allocator_types_ get_allocator_type(std::string const &user_input) {
    if (user_input == "global") {
        return data_base<key, key_comparer>::allocator_types_::global;
    } else if (user_input == "sorted_list") {
        return data_base<key, key_comparer>::allocator_types_::for_inner_use_sorted_list;
    } else if (user_input == "descriptors") {
        return data_base<key, key_comparer>::allocator_types_::for_inner_use_descriptors;
    } else if (user_input == "buddy_system") {
        return data_base<key, key_comparer>::allocator_types_::buddy_system;
    }
    return data_base<key, key_comparer>::allocator_types_::not_an_allocator;
}

std::pair<data_base<key, key_comparer>::allocator_types_, size_t>
define_allocator_type
        (data_base<key, key_comparer>::allocator_types_ allocator_type,
         std::string &s, std::string &token, std::string &delimiter,
         size_t pos, unsigned delimiter_length) {
    if (allocator_type == data_base<key, key_comparer>::allocator_types_::for_inner_use_sorted_list ||
        allocator_type == data_base<key, key_comparer>::allocator_types_::for_inner_use_descriptors) {
        s.erase(0, pos + delimiter_length);
        if ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            if (token == "best") {
                allocator_type = (allocator_type ==
                                  data_base<key, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                  ? data_base<key, key_comparer>::allocator_types_::sorted_list_best
                                  : data_base<key, key_comparer>::allocator_types_::descriptors_best);
            } else if (token == "worst") {
                allocator_type = (allocator_type ==
                                  data_base<key, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                  ? data_base<key, key_comparer>::allocator_types_::sorted_list_worst
                                  : data_base<key, key_comparer>::allocator_types_::descriptors_worst);
            } else if (token == "first") {
                allocator_type = (allocator_type ==
                                  data_base<key, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                  ? data_base<key, key_comparer>::allocator_types_::sorted_list_first
                                  : data_base<key, key_comparer>::allocator_types_::descriptors_first);
            }
        } else {
            allocator_type = data_base<key, key_comparer>::allocator_types_::not_an_allocator;
        }
    }

    size_t allocator_pool_size = 0;

    if (allocator_type != data_base<key, key_comparer>::allocator_types_::global &&
        allocator_type != data_base<key, key_comparer>::allocator_types_::not_an_allocator) {
        // need to read a size of allocator
        s.erase(0, pos + delimiter_length);
        if ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            try {
                allocator_pool_size = std::stoull(token);
                s.erase(0, pos + delimiter_length);
            }
            catch (std::invalid_argument const &) {
                throw parse_exception("define_allocator_type:: passed size of allocator must be of type size_t");
            }
        }
    }

    return {allocator_type, allocator_pool_size};
}

std::tuple<data_base<key, key_comparer>::trees_types_, data_base<key, key_comparer>::allocator_types_, size_t>
parse_for_add_command(std::string &input_str_leftover) {
    data_base<key, key_comparer>::trees_types_ tree_type = data_base<key, key_comparer>::trees_types_::not_a_tree;
    data_base<key, key_comparer>::allocator_types_ allocator_type = data_base<key, key_comparer>::allocator_types_::not_an_allocator;

    size_t allocator_pool_size = 0, pos;
    std::string token, delimiter = " ";

    unsigned delimiter_length = delimiter.length();
    if ((pos = input_str_leftover.find(delimiter)) != std::string::npos) {
        token = input_str_leftover.substr(0, pos);
        tree_type = get_tree_type(token);
        allocator_type = get_allocator_type(token);

        auto allocator_type_and_size = define_allocator_type(allocator_type, input_str_leftover, token, delimiter, pos,
                                                             delimiter_length);
        allocator_type = allocator_type_and_size.first;
        allocator_pool_size = allocator_type_and_size.second;

        if ((pos = input_str_leftover.find(delimiter)) != std::string::npos) {
            token = input_str_leftover.substr(0, pos);
            if (tree_type == data_base<key, key_comparer>::not_a_tree) {
                tree_type = get_tree_type(token);
            }

            if (allocator_type == data_base<key, key_comparer>::not_an_allocator) {
                allocator_type = get_allocator_type(token);
                allocator_type_and_size = define_allocator_type(allocator_type, input_str_leftover, token, delimiter,
                                                                pos,
                                                                delimiter_length);
                allocator_type = allocator_type_and_size.first;
                allocator_pool_size = allocator_type_and_size.second;
            }
        }
    }

    return {tree_type, allocator_type, allocator_pool_size};
}

void
do_add_command
(data_base<key, key_comparer> *db, std::string &input_str_leftover, std::ifstream *input_stream, bool is_cin)
{
    std::tuple<data_base<key, key_comparer>::trees_types_, data_base<key, key_comparer>::allocator_types_, size_t> parse_result
            = parse_for_add_command(input_str_leftover);

    data_base<key, key_comparer>::trees_types_ tree_type = std::get<0>(parse_result);
    data_base<key, key_comparer>::allocator_types_ allocator_type = std::get<1>(parse_result);
    size_t allocator_pool_size = std::get<2>(parse_result);

    std::tuple<std::string, std::string, std::string> struct_parse_path_result = parse_path(input_str_leftover);

    if (input_stream != nullptr || is_cin) {
        // adding a value
        key tmp_key(input_stream, is_cin);

        auto *dbValueBuilder = new db_value_builder();
        db_value *tmp_value = dbValueBuilder->build_from_stream(input_stream, is_cin);
        delete dbValueBuilder;

        auto path_parse_result = get_path_from_user_input(input_stream, is_cin, true);

        db->add_to_collection(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                              std::get<2>(path_parse_result),
                              tmp_key, tmp_value);
        if (is_cin) {
            std::cout << "Added a value to collection " << std::get<2>(path_parse_result) << " successfully!" << std::endl;
        }
    } else {
        // adding to structure
        db->add_to_structure(std::get<0>(struct_parse_path_result),
                             std::get<1>(struct_parse_path_result), std::get<2>(struct_parse_path_result),
                             tree_type, allocator_type, allocator_pool_size);
        std::cout << "Added " << input_str_leftover << " successfully!" << std::endl;
    }
}

#pragma endregion

#pragma region Find command

short get_part_of_data_from_input_str(std::string &str, std::string &delimiter, unsigned delimiter_length) {
    short to_return = 0;
    size_t pos;
    if ((pos = str.find(delimiter)) != std::string::npos) {
        try {
            to_return = std::stoi(str.substr(0, pos));
        }
        catch (std::invalid_argument const &) {
            throw parse_exception("get_part_of_data_from_input_str:: incorrect value for data passed. Only digits");
        }
        str.erase(0, pos + delimiter_length);
    } else {
        throw parse_exception(
                "get_part_of_data_from_input_str:: incorrect value for data passed. Format: DD/MM/YYYY hh/mm/ss");
    }
    return to_return;
}

uint64_t convert_time_str_to_ms(time_str data) {
    std::tm tmp{};
    tmp.tm_sec = data.ss;
    tmp.tm_min = data.mm; // -1 ?
    tmp.tm_hour = data.hh;
    tmp.tm_year = data.YY - 1900;
    tmp.tm_mon = data.MM;
    tmp.tm_mday = data.DD;
    uint64_t milli = std::mktime(&tmp);
    milli = milli * 1000;
    return milli;
}

uint64_t parse_time_from_input_str(std::string &input_stream) {
    time_str to_return{};

    std::string lexeme_delimiter = " ", part_delimiter = "/";
    std::string day_month_year, hour_min_sec;
    unsigned delimiter_length = 1;
    size_t pos;

    if ((pos = input_stream.find(lexeme_delimiter)) != std::string::npos) {
        day_month_year = input_stream.substr(0, pos);
        input_stream.erase(0, pos + delimiter_length);
    } else {
        throw parse_exception("parse_time_from_input_str:: should pass full data timestamp");
    }

    to_return.DD = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);
    to_return.MM = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);
    to_return.YY = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);

    to_return.hh = get_part_of_data_from_input_str(input_stream, part_delimiter, delimiter_length);
    to_return.mm = get_part_of_data_from_input_str(input_stream, part_delimiter, delimiter_length);
    to_return.ss = get_part_of_data_from_input_str(input_stream, part_delimiter, delimiter_length);

    //todo: check if data is correct
    return convert_time_str_to_ms(to_return);
}

std::tuple<db_value *, std::vector<db_value *>, db_value *>
do_find_command
(data_base<key, key_comparer> *db, std::string &input_str_leftover, std::ifstream *input_stream, bool is_cin)
{
    std::vector<db_value *> to_return_vector;
    db_value *found_with_time = nullptr, *simply_found = nullptr;

    std::string token, delimiter = " ";
//    unsigned delimiter_length = delimiter.length();
//    size_t pos;

    if (input_str_leftover.find(delimiter) != std::string::npos) {
        // with time
        uint64_t time_stamp = parse_time_from_input_str(input_str_leftover);

        key tmp_key(input_stream, is_cin);

        auto path_parse_result = get_path_from_user_input(input_stream, is_cin, true);

        found_with_time = db->find_with_time(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                                             std::get<2>(path_parse_result),
                                             tmp_key, time_stamp);
    } else if (input_str_leftover == "dataset") {
        // find in range
        if (is_cin) {
            std::cout << "Enter min and max keys" << std::endl;
        }
        key min(input_stream, is_cin);
        key max(input_stream, is_cin);

        auto path_parse_result = get_path_from_user_input(input_stream, is_cin, true);

        to_return_vector = db->find_in_range(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                                             std::get<2>(path_parse_result),
                                             min, max);
    } else {
        key tmp_key(input_stream, is_cin);
        std::tuple<std::string, std::string, std::string> path_parse_result = get_path_from_user_input(input_stream,
                                                                                                       is_cin, true);

        simply_found = db->find_among_collection(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                                                 std::get<2>(path_parse_result),
                                                 tmp_key);
    }

    return {found_with_time, to_return_vector, simply_found};
}

#pragma endregion

#pragma region Update command

std::pair<key, std::map<db_value_fields, unsigned char *>>
get_update_key_and_dictionary(std::ifstream *input_stream, bool is_cin) {
    key to_return_key(input_stream, is_cin);
    std::map<db_value_fields, unsigned char *> to_return_dict;

    std::string token, field_name, delimiter = ":";
    db_value_fields dbValueField;
    unsigned delimiter_length = delimiter.length(), iterations = 15;
    size_t pos;
    if (is_cin) {
        std::cout << "Print field_name: new_value" << std::endl
                  << "Fields: surname, name, patronymic, birthday, link_to_resume, hr_id, p_language, tasks, solved, copying"
                  << std::endl
                  << "To stop print exit" << std::endl;
    }

    while (iterations) {
        iterations--;

        if (is_cin) {
            std::getline(std::cin, token);
        } else {
            std::getline((*input_stream), token);
        }

        // delete all spaces, after this: surname:Surname
        token.erase(remove_if(token.begin(), token.end(), isspace), token.end());

        if (token == "exit") {
            break;
        }

        if ((pos = token.find(delimiter)) != std::string::npos) {
            field_name = token.substr(0, pos);
            token.erase(0, pos + delimiter_length);
        } else {
            throw parse_exception(
                    "get_update_key_and_dictionary:: incorrect input. Must be <field_name>:<new_field_value>");
        }

        if (field_name == "surname") {
            dbValueField = db_value_fields::_surname_;
        } else if (field_name == "name") {
            dbValueField = db_value_fields::_name_;
        } else if (field_name == "patronymic") {
            dbValueField = db_value_fields::_patronymic_;
        } else if (field_name == "birthday") {
            dbValueField = db_value_fields::_birthday_;
        } else if (field_name == "link_to_resume") {
            dbValueField = db_value_fields::_link_to_resume_;
        } else if (field_name == "hr_id") {
            dbValueField = db_value_fields::_hr_id_;
        } else if (field_name == "p_language") {
            dbValueField = db_value_fields::_programming_language_;
        } else if (field_name == "tasks") {
            dbValueField = db_value_fields::_task_count_;
        } else if (field_name == "solved") {
            dbValueField = db_value_fields::_solved_task_count_;
        } else if (field_name == "copying") {
            dbValueField = db_value_fields::_copying_;
        } else {
            throw parse_exception("get_update_key_and_dictionary:: incorrect field name");
        }

        if (to_return_dict.contains(dbValueField)) {
            delete to_return_dict[dbValueField];
        }

        if (dbValueField == db_value_fields::_surname_ || dbValueField == db_value_fields::_name_ ||
            dbValueField == db_value_fields::_patronymic_ ||
            dbValueField == db_value_fields::_birthday_ || dbValueField == db_value_fields::_link_to_resume_ ||
            dbValueField == db_value_fields::_programming_language_) {
            to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new std::string(token));
        } else if (dbValueField == db_value_fields::_hr_id_) {
            // try catch?
            try {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new int(std::stoi(token)));
            }
            catch (std::invalid_argument const &) {
                throw parse_exception("get_update_key_and_dictionary:: hr id value must be of type int");
            }
        } else if (dbValueField == db_value_fields::_task_count_ ||
                   dbValueField == db_value_fields::_solved_task_count_) {
            try {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new unsigned(std::stoi(token)));
            }
            catch (std::invalid_argument const &) {
                throw parse_exception("get_update_key_and_dictionary:: task count value must be of type unsigned");
            }
        } else if (dbValueField == db_value_fields::_copying_) {
            if (token == "true" || token == "1") {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new bool(true));
            } else if (token == "false" || token == "0") {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new bool(false));
            } else {
                throw parse_exception(
                        "get_update_key_and_dictionary:: incorrect copying value. It must be true/false or 1/0");
            }
        }
    }
    return {to_return_key, to_return_dict};
}

void do_update_command(data_base<key, key_comparer> *db, std::ifstream *input_stream, bool is_cin) {
    std::pair<key, std::map<db_value_fields, unsigned char *>> key_and_dict = get_update_key_and_dictionary(
            input_stream, is_cin);

    auto path_parse_result = get_path_from_user_input(input_stream, is_cin, true);
    db->update_in_collection(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                             std::get<2>(path_parse_result),
                             key_and_dict.first, key_and_dict.second);
    if (is_cin) {
        std::cout << "Updated a value in collection " << std::get<2>(path_parse_result) << " successfully!" << std::endl;
    }
}

#pragma endregion

#pragma region Delete command

void delete_db(data_base<key, key_comparer> *db) {
    db->~data_base();
}

void do_delete_command(data_base<key, key_comparer> *db, std::string &path_inner, std::ifstream *input_stream,
                       bool is_cin) {
    path_inner.erase(remove_if(path_inner.begin(), path_inner.end(), isspace), path_inner.end());

    // a key from a collection (return a value)
    if (path_inner.empty()) {
        key tmp_key(input_stream, is_cin);
        auto path_parse_result = get_path_from_user_input(input_stream, is_cin, true);

        db->delete_from_collection(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                                   std::get<2>(path_parse_result),
                                   tmp_key);
        if (is_cin) {
            std::cout << "Deleted value from collection " << std::get<2>(path_parse_result) << " successfully!" << std::endl;
        }
    }
        // delete the whole database
    else if (path_inner == "DB") {
        delete_db(db);
        if (is_cin) {
            std::cout << "Database was deleted successfully!" << std::endl;
        }
    }
        // delete pull/scheme/collection
    else {
        auto path_parse_result = parse_path(const_cast<std::string &>(path_inner));

        db->delete_from_structure(std::get<0>(path_parse_result), std::get<1>(path_parse_result),
                                  std::get<2>(path_parse_result));

        std::cout << "Removed " << path_inner << " successfully!" << std::endl;
    }
}

#pragma endregion
/*
// todo: do_save_command
void do_save_command(std::string const & path_inner, data_base * db)
{
    // path should not be empty
    if (path_inner.empty()) {
        get_path_from_user_input(false);
    } else {
        parse_path(const_cast<std::string &>(path_inner));
    }
    // save to file
}
*/
/*
// todo: do_upload_command
void do_upload_command(std::string const & path_inner, data_base * db)
{
    // path should not be empty
    if (path_inner.empty()) {
        get_path_from_user_input(false);
    } else {
        parse_path(const_cast<std::string &>(path_inner));
    }
    // upload from file
}
*/
