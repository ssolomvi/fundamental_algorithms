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
std::tuple<std::string, std::string, std::string> get_path_from_user_input(std::istringstream* input_stream, bool is_cin, bool is_path)
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

/*
// todo: add command
void do_add_command(time_t now,
                    data_base<key, db_value, key_comparer>::trees_types_ tree_type,
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

        db_value tmp_value = dbValueBuilder->build_from_stream(input_stream, is_cin, now);

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