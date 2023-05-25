#include "db/data_base.h"
#include "db_user_communication.h"

int main()
{
    time_t now = time(nullptr);


     // db value test
    auto * dbValueBuilder = new db_value_builder();
    db_value * value = nullptr;
    try {
        value = dbValueBuilder->build_from_stream(nullptr, true);
        std::cout << (*value);
    }
    catch (db_value::create_exception const &) {
        std::cout << "Incorrect input while building a value";
    }
    catch (std::invalid_argument const &) {
        std::cout << "Incorrect input while building a value";
    }
    delete dbValueBuilder;



/*
    try {
        key tmp_key(nullptr, true);
        std::cout << tmp_key;
    }
    catch (key::create_exception const &) {
        std::cout << "Incorrect input for key";
    }
    catch (std::invalid_argument const &) {
        std::cout << "Incorrect input for key";
    }
*/
    // todo: check commands non-related to data base
    /*
    std::string input;
    std::cout << "<<" ;
    std::getline(std::cin, input);
    std::tuple<
            commands_,
            data_base<key, db_value, key_comparer>::trees_types_,
            data_base<key, db_value, key_comparer>::allocator_types_,
            std::string> parsing_result;
    try {
        parsing_result = parse_user_input(input);
        auto parse_path_result = parse_path(std::get<3>(parsing_result));
        std::cout << std::get<0>(parse_path_result) << std::endl;
        std::cout << std::get<1>(parse_path_result) << std::endl;
        std::cout << std::get<2>(parse_path_result) << std::endl;
    }
    catch (parse_exception const & message) {
        std::cout << message.what();
    }
     */



/*
 // эта хрень будет выполняться, когда нам нужно будет пройти по цепочке и собрать нужную версию
 void ClientCode(Handler &handler) {
  std::vector<std::string> food = {"Nut", "Banana", "Cup of coffee"};
  for (const std::string &f : food) {
    std::cout << "Client: Who wants a " << f << "?\n";
    const std::string result = handler.Handle(f);
    if (!result.empty()) {
      std::cout << "  " << result;
    } else {
      std::cout << "  " << f << " was left untouched.\n";
    }
  }
}
 */

/*
 // эта хрень пойдёт в data_base
 int main() {
  MonkeyHandler *monkey = new MonkeyHandler;
  SquirrelHandler *squirrel = new SquirrelHandler;
  DogHandler *dog = new DogHandler;
  monkey->SetNext(squirrel)->SetNext(dog);

    std::cout << "Chain: Monkey > Squirrel > Dog\n\n";
    ClientCode(*monkey);
std::cout << "\n";
std::cout << "Subchain: Squirrel > Dog\n\n";
ClientCode(*squirrel);

delete monkey;
delete squirrel;
delete dog;
 */
    return 0;
}

/*
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
                do_add_command(now, tree_type, allocator_type, path_inner, &db);
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

//    return 0;
}
*/
/*
int main(int argc, char** argv)
{
    db_test();
}*/
