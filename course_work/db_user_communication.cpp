#include "db/data_base.h"
#include "db_user_communication.h"

void help()
{
    std::cout << "----- Course work help -----" << std::endl;
    std::cout << "Collection commands list:" << std::endl;
    std::cout << "Full path to collection: <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- add" << std::endl;
    std::cout << "\t- find" << std::endl;
    std::cout << "\t- find data set" << std::endl;
    std::cout << "\t- update" << std::endl;
    std::cout << "\t- delete" << std::endl;
    std::cout << "Structural and customization commands list:" << std::endl;
    std::cout << "Supported trees: BST (binary tree), AVL, SPLAY, RB (red-black tree)" << std::endl;
    std::cout << "Supported allocators: global, sorted_list best||worst||first, descriptors best||worst||first, buddy_system" << std::endl;
    std::cout << "\t- add <tree type> <pull_name>" << std::endl;
    std::cout << "\t- add <tree type> <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- add <tree type> <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- add <allocator type> <pull_name>" << std::endl;
    std::cout << "\t- add <allocator type> <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- add <allocator type> <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- delete <pull_name>" << std::endl;
    std::cout << "\t- delete <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- delete <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "DB commands_ list:" << std::endl;
    std::cout << "\t- save <filename>" << std::endl;
    std::cout << "\t- upload <filename>" << std::endl;
    std::cout << "\t- help" << std::endl;
    std::cout << "\t- delete DB" << std::endl;
    std::cout << "\t- exit" << std::endl;
}

commands_ get_command(std::string const & user_input)
{
    if (user_input == "add") {
        return commands_ ::_add_;
    } else if (user_input == "find_pair") {
        return commands_ ::_find_;
    } else if (user_input == "update") {
        return commands_ ::_update_;
    } else if (user_input == "delete" || user_input == "remove") {
        return commands_ ::_delete_;
    } else if (user_input == "save") {
        return commands_ ::_save_;
    } else if (user_input == "upload") {
        return commands_ ::_upload_;
    } else if (user_input == "help") {
        return commands_ ::_help_;
    } else if (user_input == "exit") {
        return commands_ ::_exit_;
    }
    return commands_::_not_a_command_;
}

data_base<key, db_value, key_comparer>::trees_types_ get_tree_type(std::string const & user_input)
{
    if (user_input == "BST" || user_input == "bst") {
        return data_base<key, db_value, key_comparer>::trees_types_ ::BST;
    } else if (user_input == "AVL" || user_input == "avl") {
        return data_base<key, db_value, key_comparer>::trees_types_ ::AVL;
    } else if (user_input == "SPLAY" || user_input == "splay") {
        return data_base<key, db_value, key_comparer>::trees_types_ ::SPLAY;
    } else if (user_input == "RB" || user_input == "rb") {
        return data_base<key, db_value, key_comparer>::trees_types_ ::RB;
    }
    return data_base<key, db_value, key_comparer>::trees_types_ ::not_a_tree;
}

data_base<key, db_value, key_comparer>::allocator_types_ get_allocator_type(std::string const & user_input)
{
    if (user_input == "global") {
        return data_base<key, db_value, key_comparer>::allocator_types_ ::global;
    } else if (user_input == "sorted_list") {
        return data_base<key, db_value, key_comparer>::allocator_types_ ::for_inner_use_sorted_list;
    } else if (user_input == "descriptors") {
        return data_base<key, db_value, key_comparer>::allocator_types_ ::for_inner_use_descriptors;
    } else if (user_input == "buddy_system") {
        return data_base<key, db_value, key_comparer>::allocator_types_ ::buddy_system;
    }
    return data_base<key, db_value, key_comparer>::allocator_types_ ::not_an_allocator;
}

// returns a command, a tree type/allocator type, path to collection
std::tuple<
        commands_,
        data_base<key, db_value, key_comparer>::trees_types_,
        data_base<key, db_value, key_comparer>::allocator_types_,
        std::string>
parse_user_input(std::string const & user_input)
{
    commands_ command = commands_ ::_not_a_command_;
    data_base<key, db_value, key_comparer>::trees_types_ tree_type = data_base<key, db_value, key_comparer>::trees_types_ ::not_a_tree;
    data_base<key, db_value, key_comparer>::allocator_types_ allocator_type = data_base<key, db_value, key_comparer>::allocator_types_ ::not_an_allocator;
//    char* path = nullptr;

    std::string token, s = user_input;
    std::string delimiter = " ";
    size_t pos;
    unsigned delimiter_length = delimiter.length();

    // finding a command
    if ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        command = get_command(token);
        s.erase(0, pos + delimiter_length);
    } else {
        command = get_command(user_input);
        s.erase();
    }
    if (command == commands_ ::_not_a_command_) {
        // incorrect user input
        throw parse_exception("Incorrect command entered");
    }

    // finding a tree type or allocator type if command is "add"
    if (command == commands_::_add_ && (pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tree_type = get_tree_type(token);
        allocator_type = get_allocator_type(token);

        if (allocator_type == data_base<key, db_value, key_comparer>::allocator_types_::for_inner_use_sorted_list ||
            allocator_type == data_base<key, db_value, key_comparer>::allocator_types_::for_inner_use_descriptors) {
            s.erase(0, pos + delimiter_length);
            if ((pos = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, pos);
                if (token == "best") {
                    allocator_type = (allocator_type == data_base<key, db_value, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                      ? data_base<key, db_value, key_comparer>::allocator_types_::sorted_list_best
                                      : data_base<key, db_value, key_comparer>::allocator_types_::descriptors_best);
                } else if (token == "worst") {
                    allocator_type = (allocator_type == data_base<key, db_value, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                      ? data_base<key, db_value, key_comparer>::allocator_types_::sorted_list_worst
                                      : data_base<key, db_value, key_comparer>::allocator_types_::descriptors_worst);
                } else if (token == "first") {
                    allocator_type = (allocator_type == data_base<key, db_value, key_comparer>::allocator_types_::for_inner_use_sorted_list
                                      ? data_base<key, db_value, key_comparer>::allocator_types_::sorted_list_first
                                      : data_base<key, db_value, key_comparer>::allocator_types_::descriptors_first);
                }
            } else {
                allocator_type = data_base<key, db_value, key_comparer>::allocator_types_ ::not_an_allocator;
            }
        }
        s.erase(0, pos + delimiter_length);
    }

    // a path is what is left from s
    return {command, tree_type, allocator_type, s};
    /*
    return std::tuple<commands_,
           data_base<key, db_value, key_comparer>::trees_types_,
           data_base<key, db_value, key_comparer>::allocator_types_,
           std::string const &>
           (command, tree_type, allocator_type, s);*/
}

std::tuple<std::string, std::string, std::string> parse_path(std::string & input_string)
{
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
std::tuple<std::string, std::string, std::string> get_path_from_user_input(std::stringstream* input_stream, bool is_cin, bool is_path)
{
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

void delete_db(data_base<key, db_value, key_comparer> * db)
{
    db->~data_base();
}

void add(db_value * value_to_add_to, db_value * new_value)
{
    handler * add_handle = new add_handler(new_value);
    handler ** last_handler = value_to_add_to->get_last_handler();
    if ((*last_handler) == nullptr) {
        // there are no commands added
        throw handler::order_exception("Add handler cannot be the first one in chain of responsibility");
//        (*last_handler) = add_handle;
    } else {
        if ((*last_handler)->get_handler_type() == handler::handler_types::_remove_handler_) {
            (*last_handler)->set_next(add_handle);
        } else {
            throw handler::order_exception("Add handler can be only after remove_handler in chain of responsibility");
        }
    }
}

std::pair<key, std::map<db_value_fields, unsigned char *>> get_update_key_and_dictionary(std::stringstream* input_stream, bool is_cin)
{
    key to_return_key(input_stream, is_cin);
    std::map<db_value_fields, unsigned char *> to_return_dict;

    std::string token, field_name, delimiter = ":";
    db_value_fields dbValueField;
    unsigned delimiter_length = delimiter.length(), iterations = 15;
    size_t pos;
    if (is_cin) {
        std::cout << "Print field_name: new_value" << std::endl
                  << "Fields: surname, name, patronymic, birthday, link_to_resume, hr_id, p_language, tasks, solved, copying" << std::endl
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
//            command = get_command(token);
            token.erase(0, pos + delimiter_length);
        } else {
            // todo: throw a message, incorrect input
        }

        if (field_name == "surname") {
            dbValueField = db_value_fields ::_surname_;
        } else if (field_name == "name") {
            dbValueField = db_value_fields ::_name_;
        } else if (field_name == "patronymic") {
            dbValueField = db_value_fields ::_patronymic_;
        } else if (field_name == "birthday") {
            dbValueField = db_value_fields ::_birthday_;
        } else if (field_name == "link_to_resume") {
            dbValueField = db_value_fields ::_link_to_resume_;
        } else if (field_name == "hr_id") {
            dbValueField = db_value_fields ::_hr_id_;
        } else if (field_name == "p_language") {
            dbValueField = db_value_fields ::_programming_language_;
        } else if (field_name == "tasks") {
            dbValueField = db_value_fields ::_task_count_;
        } else if (field_name == "solved") {
            dbValueField = db_value_fields ::_solved_task_count_;
        } else if (field_name == "copying") {
            dbValueField = db_value_fields ::_copying_;
        } else {
            // todo: throw a message incorrect input
        }

        if (to_return_dict.contains(dbValueField)) {
            delete to_return_dict[dbValueField];
        }

        if (dbValueField == db_value_fields::_surname_ || dbValueField == db_value_fields::_name_ || dbValueField == db_value_fields::_patronymic_ ||
            dbValueField == db_value_fields::_birthday_ || dbValueField == db_value_fields::_link_to_resume_ || dbValueField == db_value_fields::_programming_language_) {
            to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new std::string(token));
        } else if (dbValueField == db_value_fields::_hr_id_) {
            // try catch?
            to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new int(std::stoi(token)));
        } else if (dbValueField == db_value_fields::_task_count_ || dbValueField == db_value_fields::_solved_task_count_) {
            // todo: try catch?
            to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new unsigned(std::stoi(token)));
        } else if (dbValueField == db_value_fields::_copying_) {
            if (token == "true" || token == "1") {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new bool(true));
            } else if (token == "false" || token == "0") {
                to_return_dict[dbValueField] = reinterpret_cast<unsigned char *>(new bool(false));
            } else {
                // throw a message, parse error
            }
        }
    }
    return {to_return_key, to_return_dict};
    // check if map is not empty ?
}

// dict will be got in some parse function
void update(db_value * value_to_update_to, std::map<db_value_fields, unsigned char *> dict)
{
    handler * update_handle = new update_handler(std::move(dict));
    handler ** last_handler = value_to_update_to->get_last_handler();
    if ((*last_handler) == nullptr) {
        (*last_handler) = update_handle;
    } else {
        if ((*last_handler)->get_handler_type() != handler::handler_types::_remove_handler_) {
            ((*last_handler)->set_next(update_handle));
        } else {
            throw handler::order_exception("Update handler cannot be after remove_handler in chain of responsibility");
        }
    }
}

void remove(db_value * value_to_delete)
{
    handler * delete_handle = new remove_handler();
    handler ** last_handler = value_to_delete->get_last_handler();
    if ((*last_handler) == nullptr) {
        (*last_handler) = delete_handle;
    } else {
        if ((*last_handler)) {
            ((*last_handler)->set_next(delete_handle));
        }
    }
}

db_value * find_value_version_time(db_value * value, uint64_t time_parameter)
{
    db_value * value_copy = value->make_a_copy();

    handler * first_handler = value->get_first_handler();

    if (first_handler == nullptr) {
        return value_copy;
    }
    // будут ли value_copy и result_handler указывать на одно и то же? по идее должны
    db_value * result_handler = value->get_first_handler()->handle(value_copy, time_parameter);
//    delete value_copy;
    return value_copy;
}

short get_part_of_data_from_input_str(std::string & str, std::string & delimiter, unsigned delimiter_length)
{
    short to_return = 0;
    size_t pos;
    if ((pos = str.find(delimiter)) != std::string::npos) {
        try {
            to_return = std::stoi(str.substr(0, pos));
        }
        catch (std::invalid_argument const &) {
            // todo: throw incorrect input
        }
        str.erase(0, pos + delimiter_length);
    } else {
        // todo: throw incorrect input
    }
    return to_return;
}

uint64_t parse_time_from_stream(std::stringstream * input_stream, bool is_cin)
{
    time_str to_return{};

    std::string token;
    if (is_cin) {
        std::cout << "Print timestamp in format {DD/MM/YY hh/mm/ss} <<";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }

    std::string lexeme_delimiter = " ", part_delimiter = "/";
    std::string day_month_year, hour_min_sec;
    unsigned delimiter_length = 1;
    size_t pos;

    if ((pos = token.find(lexeme_delimiter)) != std::string::npos) {
        day_month_year = token.substr(0, pos);
        token.erase(0, pos + delimiter_length);
    } else {
        // todo: throw incorrect input
    }

    to_return.DD = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);
    to_return.MM = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);
    to_return.YY = get_part_of_data_from_input_str(day_month_year, part_delimiter, delimiter_length);

    to_return.hh = get_part_of_data_from_input_str(token, part_delimiter, delimiter_length);
    to_return.mm = get_part_of_data_from_input_str(token, part_delimiter, delimiter_length);
    to_return.ss = get_part_of_data_from_input_str(token, part_delimiter, delimiter_length);

    //todo: check if data is correct
    return convert_time_str_to_ms(to_return);
}

uint64_t convert_time_str_to_ms(time_str data)
{
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

/*
// todo: add command
void do_add_command(data_base<key, db_value, key_comparer>::trees_types_ tree_type,
                    data_base<key, db_value, key_comparer>::allocator_types_ allocator_type,
                    std::string const & path_inner,
                    std::istringstream* input_stream, bool is_cin,
                    data_base<key, db_value, key_comparer>* db)
{
    std::tuple<std::string, std::string, std::string> path_parse_result;

    if (path_inner.empty()) {
        // 1. key/value to collection
        key tmp_key(input_stream, is_cin);

        db_value_builder *dbValueBuilder = new db_value_builder();

        db_value * tmp_value = dbValueBuilder->build_from_stream(input_stream, is_cin);

        delete dbValueBuilder;

        path_parse_result = get_path_from_user_input(input_stream, is_cin, true);
        db->add_to_collection(std::get<0>(path_parse_result),
                              std::get<1>(path_parse_result),
                              std::get<2>(path_parse_result),
                              tmp_key, std::move(tmp_value));
    }
    else {
        // 2. pull, scheme, collection (path is not empty)
        path_parse_result = parse_path(const_cast<std::string &>(path_inner));
        db->add_to_structure(std::get<0>(path_parse_result),
                             std::get<1>(path_parse_result),
                             std::get<2>(path_parse_result),
                             tree_type, allocator_type);
        // print to console "successfully/not added to ..."
        // todo: catch. If not catch, print success.
        std::cout << "Added " << path_inner << " successfully!" << std::endl;
    }
}
*/
/*
// todo: do_find_command
const db_value& do_find_command(
        std::istringstream* input_stream, bool is_cin,
        data_base<key, db_value, key_comparer> * db)
{
    // 1. ask for key
    key tmp_key(input_stream, is_cin);
    // 2. ask for path
    std::tuple<std::string, std::string, std::string> path_parse_result = get_path_from_user_input(input_stream, is_cin, true);

    auto find_result = db->find_among_collection(std::get<0>(path_parse_result),
                                                     std::get<1>(path_parse_result),
                                                     std::get<2>(path_parse_result),
                                                     tmp_key);
    return find_result;
}
*/
/*
// todo: do_update_command
void do_update_command(time_t now, data_base<key, db_value, key_comparer> * db)
{
    // 1. ask for key, value
    key tmp_key(input_stream, is_cin);
    db_value tmp_value = get_value_from_user_input();

    // 2. ask for path
    std::tuple<std::string, std::string, std::string> path_parse_result = get_path_from_user_input(true);

    // 3. generate a new_key with <key.id, key.id, time::now>
    data_base::key_struct new_key = key.generate_new_key(now);

    db->update
}

// todo: do_delete_command
void do_delete_command(std::string const & path_inner, data_base * db)
{
    // a key from a collection (return a value)
    if (path_inner.empty()) {
        /*
        get_key_from_user_input();

        get_path_from_user_input();
    }
        // delete the whole database
    else if (path_inner == "DB") {
        delete_db(db);
        std::cout << "Database was deleted successfully!" << std::endl;
    }
        // delete pull/scheme/collection
    else {
        parse_path(const_cast<std::string &>(path_inner));
        std::cout << "Removed " << path_inner << " successfully!" << std::endl;
    }
}

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
