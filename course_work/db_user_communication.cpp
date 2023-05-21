#include "db/data_base.h"
#include "db_user_communication.h"

void help()
{
    std::cout << "----- Course work help -----" << std::endl;
    std::cout << "Collection commands_ list:" << std::endl;
    std::cout << "Full path to collection: <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- add" << std::endl;
    std::cout << "\t- find" << std::endl;
    std::cout << "\t- find data set" << std::endl;
    std::cout << "\t- update" << std::endl;
    std::cout << "\t- delete" << std::endl;
    std::cout << "Structural commands_ list:" << std::endl;
    std::cout << "\t- add/delete <pull_name>" << std::endl;
    std::cout << "\t- add/delete <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- add/delete <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "Structure customization commands_ list:" << std::endl;
    std::cout << "Supported trees: BST (binary tree), AVL, SPLAY, RB (red-black tree)" << std::endl;
    std::cout << "Supported allocators: global, sorted_list best||worst||first, descriptors best||worst||first, buddy_system" << std::endl;
    std::cout << "\t- change <tree type> <pull_name>" << std::endl;
    std::cout << "\t- change <tree type> <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- change <tree type> <pull_name>/<scheme_name>/<collection_name>" << std::endl;
    std::cout << "\t- change <allocator type> <pull_name>" << std::endl;
    std::cout << "\t- change <allocator type> <pull_name>/<scheme_name>" << std::endl;
    std::cout << "\t- change <allocator type> <pull_name>/<scheme_name>/<collection_name>" << std::endl;
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
    } else if (user_input == "find") {
        return commands_ ::_find_;
    } else if (user_input == "update") {
        return commands_ ::_update_;
    } else if (user_input == "delete" || user_input == "remove") {
        return commands_ ::_delete_;
    } else if (user_input == "change") {
        return commands_ ::_change_;
    } else if (user_input == "save") {
        return commands_ ::_save_;
    } else if (user_input == "upload") {
        return commands_ ::_upload_;
    } else if (user_input == "help") {
        return commands_ ::_help_;
    } else if (user_input == "exit") {
        return commands_ ::_exit_;
    }
    /*
    if (strcmp(user_input, "") == 0) {

    }
     */
    return commands_::_not_a_command_;
}

data_base::trees_types_ get_tree_type(std::string const & user_input)
{
    if (user_input == "BST" || user_input == "bst") {
        return data_base::trees_types_ ::BST;
    } else if (user_input == "AVL" || user_input == "avl") {
        return data_base::trees_types_ ::AVL;
    } else if (user_input == "SPLAY" || user_input == "splay") {
        return data_base::trees_types_ ::SPLAY;
    } else if (user_input == "RB" || user_input == "rb") {
        return data_base::trees_types_ ::RB;
    }
    return data_base::trees_types_ ::not_a_tree;
}

data_base::allocator_types_ get_allocator_type(std::string const & user_input)
{
    if (user_input == "global") {
        return data_base::allocator_types_ ::global;
    } else if (user_input == "sorted_list") {
        return data_base::allocator_types_ ::for_inner_use_sorted_list;
    } else if (user_input == "descriptors") {
        return data_base::allocator_types_ ::for_inner_use_descriptors;
    } else if (user_input == "buddy_system") {
        return data_base::allocator_types_ ::buddy_system;
    }
    return data_base::allocator_types_ ::not_an_allocator;
}

std::tuple<commands_, data_base::trees_types_, data_base::allocator_types_, std::string const &> parse_user_input(std::string const & user_input)
{
    commands_ command = commands_ ::_not_a_command_;
    data_base::trees_types_ tree_type = data_base::trees_types_ ::not_a_tree;
    data_base::allocator_types_ allocator_type = data_base::allocator_types_ ::not_an_allocator;
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
        return std::tuple<commands_, data_base::trees_types_, data_base::allocator_types_, std::string const &>(command, tree_type, allocator_type, s);
    }

    // finding a tree type or allocator type if command is "change"
    if (command == commands_::_change_ && (pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tree_type = get_tree_type(token);
        allocator_type = get_allocator_type(token);

        if (allocator_type == data_base::allocator_types_::for_inner_use_sorted_list || allocator_type == data_base::allocator_types_::for_inner_use_descriptors) {
            s.erase(0, pos + delimiter_length);
            if ((pos = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, pos);
                if (token == "best") {
                    allocator_type = (allocator_type == data_base::allocator_types_::for_inner_use_sorted_list
                                      ? data_base::allocator_types_::sorted_list_best
                                      : data_base::allocator_types_::descriptors_best);
                } else if (token == "worst") {
                    allocator_type = (allocator_type == data_base::allocator_types_::for_inner_use_sorted_list
                                      ? data_base::allocator_types_::sorted_list_worst
                                      : data_base::allocator_types_::descriptors_worst);
                } else if (token == "first") {
                    allocator_type = (allocator_type == data_base::allocator_types_::for_inner_use_sorted_list
                                      ? data_base::allocator_types_::sorted_list_first
                                      : data_base::allocator_types_::descriptors_first);
                }
            } else {
                allocator_type = data_base::allocator_types_ ::not_an_allocator;
            }
        }
        s.erase(0, pos + delimiter_length);
    }

    // a path is what is left from s
    return std::tuple<commands_, data_base::trees_types_, data_base::allocator_types_, std::string const &>(command, tree_type, allocator_type, s);
}

data_base::key_struct get_key_from_user_input(time_t now)
{
    data_base::key_struct new_key;

    std::string key, delimiter = ", ";
    std::string applicant_id, contest_id;
    size_t pos;
    unsigned delimiter_length = delimiter.length();

    std::cout << "Enter key:\napplicant id, contest id: >> ";
    std::getline(std::cin, key);

    if ((pos = key.find(delimiter)) != std::string::npos) {
        applicant_id = key.substr(0, pos);
        key.erase(0, pos + delimiter_length);
    } else {
        throw data_base::db_key_exception("Incorrect input for key");
    }

    if (key.empty()) {
        throw data_base::db_key_exception("Incorrect input for key");
    }

    contest_id = key;

    new_key.applicant_id = stoi(applicant_id);
    new_key.contest_id = stoi(applicant_id);
    new_key.utc_time = gmtime(&now);
    // to get the time as a string: dt = asctime(new_key.utc_time);
}

// TODO: get_value_from_user_input
data_base::value_struct get_value_from_user_input()
{
    // todo: add prefix_tree (?) to store strings as char *
    // for that it's needed (?) to have a value_builder class/structure, which will have an access to this prefix_tree
    // but what about deletion (?)
    std::string surname, name, patronymic;
    std::string birthday, link_to_resume;
    unsigned id_hr;
    std::string programming_language;
    unsigned task_count, solved_task_count;
    bool copying;
    std::string value;
    std::cout << "Enter value:" << std::endl;
    // todo: parse values
    // todo: make a constructor of value (?) by using a builder structure
    std::cout << "Surname Name Patronymic:              >> ";
    std::getline(std::cin, value);
    std::cout << "Birthday <dd/mm/year>:                >> ";
    std::getline(std::cin, value);
    std::cout << "Link to resume:                       >> ";
    std::getline(std::cin, value);
    std::cout << "id of HR-manager:                     >> ";
    std::getline(std::cin, value);
    std::cout << "Programming language:                 >> ";
    std::getline(std::cin, value);
    std::cout << "Task count: {whole number}            >> ";
    std::getline(std::cin, value);
    std::cout << "Solved tasks count {whole number}:    >> ";
    std::getline(std::cin, value);
    std::cout << "Copying with a micro-earphone {bool}: >> ";
    std::getline(std::cin, value);
}

std::tuple<std::string, std::string, std::string> parse_path(std::string & input_string)
{
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
    return std::tuple<std::string, std::string, std::string>(pull_name, scheme_name, collection_name);
}

// returns pull/scheme/collection
std::tuple<std::string, std::string, std::string> get_path_from_user_input()
{
    std::string path_inner;

    std::cout << "Enter full path to collection: >> ";
    std::getline(std::cin, path_inner);
    return parse_path(path_inner);
}

// todo: delete_db
void delete_db(data_base * db)
{

}

// todo: do_add_command
void do_add_command(time_t now, std::string const & path_inner, data_base* db)
{
    if (path_inner.empty()) {
        // 1. key/value to collection
        data_base::key_struct tmp_key = get_key_from_user_input(now);

        data_base::value_struct tmp_value = get_value_from_user_input();

        auto get_path_result = get_path_from_user_input();
        db->add_to_collection(std::get<0>(get_path_result), std::get<1>(get_path_result), std::get<2>(get_path_result), tmp_key, tmp_value);
    }
    else {
        // 2. pull, scheme, collection (path is not empty)
        // print to console "successfully/not added to ..."
        std::cout << "Added " << path_inner << " successfully!" << std::endl;
    }
}

// todo: do_find_command
void do_find_command(data_base * db)
{
    // 1. ask for key
    get_key_from_user_input();
    // 2. ask for path
    get_path_from_user_input();
}

// todo: do_update_command
void do_update_command(data_base * db)
{
    // 1. ask for key, value
    get_key_from_user_input();
    get_value_from_user_input();

    // 2. ask for path
    get_path_from_user_input();
}

// todo: do_delete_command
void do_delete_command(std::string const & path_inner, data_base * db)
{
    // a key from a collection (return a value)
    if (path_inner.empty()) {
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

// todo: do_change_command
void do_change_command(data_base::trees_types_ tree_type, data_base::allocator_types_ allocator_type, std::string const & path_inner, data_base * db)
{
    std::tuple<std::string, std::string, std::string> path_parse_result;
    // path must not be empty
    if (path_inner.empty()) {
        path_parse_result = get_path_from_user_input();
    } else {
        path_parse_result = parse_path(const_cast<std::string &>(path_inner));
    }
    // change. If not exists, add(?)
}

// todo: do_save_command
void do_save_command(std::string const & path_inner, data_base * db)
{
    // path should not be empty
    if (path_inner.empty()) {
        get_path_from_user_input();
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
        get_path_from_user_input();
    } else {
        parse_path(const_cast<std::string &>(path_inner));
    }
    // upload from file
}
