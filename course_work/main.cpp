#include "db/data_base.h"
#include "db_user_communication.h"

void db_test()
{
    // создать бд с дефолтными деревьями (?)
    data_base db;

    time_t now = time(0);

    help();
    bool not_exited = true;

    std::string user_input;
    while (not_exited) {
        std::cout << ">> ";
        std::getline(std::cin, user_input);

        auto result_of_user_input_parse = parse_user_input(user_input);
        commands_ command = std::get<0>(result_of_user_input_parse);
        data_base::trees_types_ tree_type = std::get<1>(result_of_user_input_parse);
        data_base::allocator_types_ allocator_type = std::get<2>(result_of_user_input_parse);
        std::string path_inner = std::get<3>(result_of_user_input_parse);

        switch (command) {
            case commands_::_add_:
                do_add_command(now, path_inner, &db);
                break;
            case commands_::_find_:
                do_find_command(&db);
                break;
            case commands_::_update_:
                do_update_command(&db);
                break;
            case commands_::_delete_:
                do_delete_command(path_inner, &db);
                break;
            case commands_::_change_:
                do_change_command(tree_type, allocator_type, path_inner, &db);
                break;
            case commands_::_save_:
                do_save_command(path_inner, &db);
                break;
            case commands_::_upload_:
                do_upload_command(path_inner, &db);
                break;
            case commands_::_help_:
                help();
                break;
            case commands_::_exit_:
                // todo: remove db
                delete_db(&db);
                std::cout << "Exited successfully!" << std::endl;
                not_exited = false;
                break;
            default:
                std::cout << "Wrong command passed, try again!" << std::endl;
                break;
        }
    }

/*
    if (argc != 2) {
        // команды будут поступать не из файла, а с консоли,
        std::cout << "You choose:\n>>";
    } else {
        // прочитать файл
    }
*/

//    return 0;
}

int main(int argc, char** argv)
{
    db_test();
}
