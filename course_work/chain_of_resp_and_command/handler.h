#ifndef HANDLER_H
#define HANDLER_H

//#include "../db_value/db_value.h"
#include "command.h"
#include <chrono>

//class db_value;

class handler {
public:
    enum handler_types {
        _add_handler_,
        _update_handler_,
        _remove_handler_
    };

    class order_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit order_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

protected:
    handler * _next_handler;
    command * _command;
    uint64_t timestamp;
    handler_types _type_of_handler;

public:
    [[nodiscard]] uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    [[nodiscard]] handler_types get_handler_type() const
    {
        return this->_type_of_handler;
    }

    handler *set_next(handler *handler)
    {
        this->_next_handler = handler;
        return handler;
    }

    // this handler (the first in chain) cannot be proved to be last in this function. Must do a check-up in invoker function
    handler ** find_last_handler(handler * invoker_handler) const
    {
        if (invoker_handler->_next_handler == nullptr) {
            return nullptr;
        }

        handler **last_handler = &(invoker_handler->_next_handler);

        while ((*last_handler)->_next_handler != nullptr) {
            last_handler = &((*last_handler)->_next_handler);
        }

        return last_handler;
    }

public:
    db_value* handle(db_value * request, uint64_t time_parameter)
    {
        if (this->timestamp <= time_parameter) {
            request = _command->execute(request);
        } else {
            return request;
        }
        // do command

        if (this->_next_handler != nullptr) {
            return this->_next_handler->handle(request, time_parameter);
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
public:
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
        _type_of_handler = handler_types::_add_handler_;
    }
};

class update_handler final : public handler
{
public:
    explicit update_handler(std::map<db_value_fields, unsigned char *> command_input)
    {
        _command = new update_command(std::move(command_input));
        _type_of_handler = handler_types::_update_handler_;
    }
};

class remove_handler final : public handler
{
public:
    remove_handler()
    {
        _command = new remove_command();
        _type_of_handler = handler_types::_remove_handler_;
    }
};

#endif //HANDLER_H
