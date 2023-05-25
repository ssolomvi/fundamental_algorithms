#ifndef HANDLER_H
#define HANDLER_H

#include "../db_value/db_value.h"
#include "command.h"
#include <chrono>

class db_value;

class handler {
protected:
    handler * _next_handler;
    command * _command;
    uint64_t timestamp;

    public:
    [[nodiscard]] uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    handler *set_next(handler *handler)
    {
        this->_next_handler = handler;
        return handler;
    }

    handler * find_last_handler()
    {
        handler * last_handler = this;

        while (last_handler->_next_handler != nullptr) {
            last_handler = last_handler->_next_handler;
        }
        return last_handler;
    }

public:
    db_value* handle(db_value * request, uint64_t time_parameter)
    {
        // do command
        request = _command->execute(request);

        if (this->_next_handler) {
            if ((this->_next_handler)->get_timestamp() <= time_parameter) {
                return this->_next_handler->handle(request, time_parameter);
            }
        }

        return request;
    }

public:
    explicit handler()
            : _next_handler(nullptr), _command(nullptr)
    {
        timestamp = duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void delete_chain_of_responsibility()
    {
        handler * next = this->_next_handler;
        this->~handler();
        if (next != nullptr) {
            next->delete_chain_of_responsibility();
        }
    }

    ~handler()
    {
        delete _command;
    }
};

class add_handler final : public handler
{
    // принимает на вход АБСОЛЮТНО новое значение, никак не связанное с initial db_value
    explicit add_handler(db_value * new_value)
    {
        // собираем словарь для команды add
        std::map<db_value_fields, unsigned char *> command_input;

        command_input[db_value_fields::_surname_] =
                reinterpret_cast<unsigned char *>(new std::string((*(new_value->_surname))));
        command_input[db_value_fields::_name_] =
                reinterpret_cast<unsigned char *>(new std::string((*(new_value->_name))));
        command_input[db_value_fields::_patronymic_] =
                reinterpret_cast<unsigned char *>(new std::string(*(new_value->_patronymic)));
        command_input[db_value_fields::_birthday_] =
                reinterpret_cast<unsigned char *>(new std::string(*(new_value->_birthday)));
        command_input[db_value_fields::_link_to_resume_] =
                reinterpret_cast<unsigned char *>(new std::string(*(new_value->_link_to_resume)));
        command_input[db_value_fields::_hr_id_] =
                reinterpret_cast<unsigned char *>(new int(new_value->_hr_id));
        command_input[db_value_fields::_programming_language_] =
                reinterpret_cast<unsigned char *>(new std::string(*(new_value->_programming_language)));
        command_input[db_value_fields::_task_count_] =
                reinterpret_cast<unsigned char *>(new unsigned(new_value->_task_count));
        command_input[db_value_fields::_solved_task_count_] =
                reinterpret_cast<unsigned char *>(new unsigned(new_value->_solved_task_count));
        command_input[db_value_fields::_copying_] =
                reinterpret_cast<unsigned char *>(new bool(new_value->_copying));

        _command = new add_command(command_input);
    }
};

class update_handler final : public handler
{
    explicit update_handler(std::map<db_value_fields, unsigned char *> command_input)
    {
        _command = new update_command(std::move(command_input));
    }
};

class remove_handler final : public handler
{
    remove_handler()
    {
        _command = new remove_command();
    }
};


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

#endif //HANDLER_H
