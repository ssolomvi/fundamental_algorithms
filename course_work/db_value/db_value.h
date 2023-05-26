#ifndef DB_VALUE_H
#define DB_VALUE_H

#include "string_holder.h"

typedef enum fields {
    _surname_,
    _name_,
    _patronymic_,
    _birthday_,
    _link_to_resume_,
    _hr_id_,
    _programming_language_,
    _task_count_,
    _solved_task_count_,
    _copying_
} db_value_fields;

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

    uint64_t _timestamp;

    handler * _chain_of_resp;

    friend class db_value_builder;
    friend class update_command;
    friend class add_command;
    friend class add_handler;

public:
    handler * get_first_handler();
    handler ** get_last_handler();

public:
    friend std::ostream &operator<<(std::ostream &out, const db_value &value);

    db_value * make_a_copy()
    {
        db_value * copy = new db_value((*(this->_surname)), (*(this->_name)), (*(this->_patronymic)),
                                       (*(this->_birthday)), (*(this->_link_to_resume)), this->_hr_id,
                                       (*(this->_programming_language)), this->_task_count, this->_solved_task_count,
                                       this->_copying);
        copy->_timestamp = this->_timestamp;
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
             std::string const & prog_lang, unsigned task_count, unsigned solved_task_count, bool copying)
            : _hr_id(hr_id), _task_count(task_count), _solved_task_count(solved_task_count), _copying(copying)
    {
        _surname = get_ptr_from_string_holder(surname);
        _name = get_ptr_from_string_holder(name);
        _patronymic = get_ptr_from_string_holder(patronymic);
        _birthday = get_ptr_from_string_holder(birthday);
        _link_to_resume = get_ptr_from_string_holder(link_to_resume);
        _programming_language = get_ptr_from_string_holder(prog_lang);

        _timestamp = duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
        _chain_of_resp = nullptr;
    }

public:
    ~db_value();
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
    out << value._timestamp << std::endl;
    return out;
}

#endif //DB_VALUE_H
