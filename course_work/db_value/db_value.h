#ifndef COURSE_WORK_DB_VALUE_H
#define COURSE_WORK_DB_VALUE_H

#include "../avl_tree/avl_tree.h"

class db_value {
public:
    class string_comparer
    {
    public:
        int operator()(std::string const & x, std::string const & y) {
            return x.compare(y);
        }
    };

private:
    std::string * _surname, * _name, * _patronymic;
    std::string * _birthday;
    std::string * _link_to_resume;
    unsigned _hr_id;
    std::string * _programming_language;
    unsigned _task_count;
    unsigned _solved_task_count;
    bool _copying;

    static avl_tree<std::string, unsigned, string_comparer>* string_holder;

private:
    std::string * get_ptr_from_string_holder(std::string const & s)
    {
        // try to find a string in string holder.
        // If it is exists, update value unsigned +1, get ptr to key
        bs_tree<std::string, unsigned, string_comparer>::node* find_result = nullptr;
        try {
            find_result = string_holder->find(s);
        } catch (bs_tree<std::string, unsigned, string_comparer>::find_exception const &) {
            // not found, insert a new key with value 1
            unsigned uses = 1;
            string_holder->insert(s, std::move(uses));
            find_result = string_holder->find(s);
            return &(find_result->key);
        }

        find_result->value += 1;
        return &(find_result->key);
    }

    void remove_string_from_string_holder(std::string const & s)
    {
        bs_tree<std::string, unsigned, string_comparer>::node* find_result = string_holder->find(s);
        if (find_result->value == 1) {
            string_holder->remove(s);
        } else {
            find_result->value--;
        }
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
    }



    ~db_value()
    {
        remove_string_from_string_holder((*_surname));
        remove_string_from_string_holder((*_name));
        remove_string_from_string_holder((*_patronymic));
        remove_string_from_string_holder((*_birthday));
        remove_string_from_string_holder((*_link_to_resume));
        remove_string_from_string_holder((*_programming_language));
    }
};

avl_tree<std::string, unsigned, db_value::string_comparer>* db_value::string_holder = new avl_tree<std::string, unsigned, db_value::string_comparer>();

#endif //COURSE_WORK_DB_VALUE_H
