#include "db/data_base.h"
#include "db_user_communication.h"

void db_test(data_base<key, key_comparer> * db, std::ifstream *input_stream, bool is_cin) {
    if (is_cin) {
        help();
    }
    bool not_exited = true;
    std::string user_input;

    while (not_exited) {
        if (is_cin) {
            std::cout << ">>";
            std::getline(std::cin, user_input);
        } else {
            std::getline((*input_stream), user_input);
        }

        auto command_and_leftover_string = parse_user_input(user_input);
        commands_ command = command_and_leftover_string.first;
        std::string leftover = command_and_leftover_string.second;

        switch (command) {
            case commands_::_add_:
                try {
                    do_add_command(db, leftover, input_stream, is_cin);
                }
                catch (declaration) {

                }
                break;
            case commands_::_find_:
                do_find_command(db, leftover, input_stream, is_cin);
                break;
            case commands_::_update_:
                do_update_command(db, input_stream, is_cin);
                break;
            case commands_::_delete_:
                do_delete_command(db, leftover, input_stream, is_cin);
                break;
//            case commands_::_save_:
//                do_save_command(path_inner, &db);
//                break;
//            case commands_::_upload_:
//                do_upload_command(path_inner, &db);
//                break;
            case commands_::_help_:
                help();
                break;
            case commands_::_exit_:
                delete_db(db);
                std::cout << "Exited successfully!" << std::endl;
                not_exited = false;
                break;
            default:
                std::cout << "Wrong command passed, try again!" << std::endl;
                break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "You should point a path to file with execution commands as a command line argument for proper work of program" << std::endl;
        return 0;
    }

    auto * file = new std::ifstream(argv[1]);
    if ( !(*file) ) {
        std::cout << "Could not open a file " << argv[1] << std::endl;
        return 0;
    }

    data_base<key, key_comparer> db;

    db_test(&db, file, false);

    std::cout << "Would you like to get some closer interaction with my program? Print y for yes an n for no\n>>";
    std::string answer_to_important_question;
    std::getline(std::cin, answer_to_important_question);

    if (answer_to_important_question == "y") {
        db_test(&db, nullptr, true);
    }
    return 0;
}

/*
    if (argc != 2) {
        // команды будут поступать не из файла, а с консоли,
        std::cout << "You choose:\n>>";
    } else {
        // прочитать файл
    }

//    return 0;
}
int main(int argc, char** argv)
{
    db_test();
}*/
