#ifndef DB_VALUE_H
#define DB_VALUE_H

#include "string_holder.h"
//#include "../chain_of_resp_and_command/handler.h"

class handler;

class db_value {
public:
    class create_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit create_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

private:
    std::string * _surname, * _name, * _patronymic;
    std::string * _birthday;
    std::string * _link_to_resume;
    int _hr_id;
    std::string * _programming_language;
    unsigned _task_count;
    unsigned _solved_task_count;
    bool _copying;

    tm * _timestamp;

    handler * chain_of_resp;

    friend class db_value_builder;
    friend class update_command;
    friend class add_command;
    friend class add_handler;
public:
    friend std::ostream &operator<<(std::ostream &out, const db_value &value);

    db_value * make_a_copy()
    {
        db_value * copy = new db_value((*(this->_surname)), (*(this->_name)), (*(this->_patronymic)),
                                       (*(this->_birthday)), (*(this->_link_to_resume)), this->_hr_id,
                                       (*(this->_programming_language)), this->_task_count, this->_solved_task_count,
                                       this->_copying, 0);
        copy->_timestamp = new tm((*(this->_timestamp)));
        return copy;
    }

private:
    static std::string * get_ptr_from_string_holder(std::string const & s)
    {
        return string_holder::get_instance()->get_string(s);
    }

    static void remove_string_from_string_holder(std::string const & s)
    {
        string_holder::get_instance()->remove_string(s);
    }

    db_value(std::string const & surname, std::string const & name, std::string const & patronymic,
             std::string const & birthday, std::string const & link_to_resume, unsigned hr_id,
             std::string const & prog_lang, unsigned task_count, unsigned solved_task_count, bool copying, time_t now)
            : _hr_id(hr_id), _task_count(task_count), _solved_task_count(solved_task_count), _copying(copying)
    {
        _surname = get_ptr_from_string_holder(surname);
        _name = get_ptr_from_string_holder(name);
        _patronymic = get_ptr_from_string_holder(patronymic);
        _birthday = get_ptr_from_string_holder(birthday);
        _link_to_resume = get_ptr_from_string_holder(link_to_resume);
        _programming_language = get_ptr_from_string_holder(prog_lang);

        _timestamp = gmtime(&now);
    }

public:
    ~db_value()
    {
        remove_string_from_string_holder((*_surname));
        remove_string_from_string_holder((*_name));
        remove_string_from_string_holder((*_patronymic));
        remove_string_from_string_holder((*_birthday));
        remove_string_from_string_holder((*_link_to_resume));
        remove_string_from_string_holder((*_programming_language));
        // ???
        delete _timestamp;
    }
};

inline std::ostream &operator<<(std::ostream &out, const db_value &value) {
    out << (*(value._surname)) << " " << (*(value._name)) << " " << (*(value._patronymic)) << std::endl;
    out << (*(value._birthday)) << std::endl;
    out << (*(value._link_to_resume)) << std::endl;
    out << value._hr_id << std::endl;
    out << (*(value._programming_language)) << std::endl;
    out << value._task_count << std::endl;
    out << value._solved_task_count << std::endl;
    out << value._copying << std::endl;
    out << asctime(value._timestamp) << std::endl;
    return out;
}

#endif //DB_VALUE_H
