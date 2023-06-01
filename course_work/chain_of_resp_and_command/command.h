#ifndef COURSE_WORK_COMMAND_H
#define COURSE_WORK_COMMAND_H

#include "../db_value/db_value.h"
#include "../db_value/db_value_builder.h"

class command {
public:
    virtual ~command() = default;
    virtual db_value ** execute(db_value **) = 0;
};

class remove_command final : public command
{
public:
    db_value ** execute(db_value ** initial_value_copy) override
    {
        std::cout << "remove command" << std::endl;
        (*initial_value_copy)->~db_value();
        (*initial_value_copy) = nullptr;
        return initial_value_copy;
    }
};

class update_command final : public command {
    std::map<db_value_fields, unsigned char *> _update_dictionary;
public:
    explicit update_command(std::map<db_value_fields, unsigned char *> dictionary)
    : _update_dictionary(std::move(dictionary))
    {

    }

    ~update_command() override {
        // do delete void * for all items in dictionary
        for (const auto & iter : _update_dictionary) {
            delete iter.second;
        }
    }

    db_value ** execute(db_value ** initial_value_copy) override
    {
        std::cout << "update command" << std::endl;

        for (const auto & iter : _update_dictionary) {
            switch (iter.first) {
                case db_value_fields::_surname_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_surname)));
                    (*initial_value_copy)->_surname = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_name_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_name)));
                    (*initial_value_copy)->_name = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_patronymic_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_patronymic)));
                    (*initial_value_copy)->_patronymic = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_birthday_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_birthday)));
                    (*initial_value_copy)->_birthday = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_link_to_resume_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_link_to_resume)));
                    (*initial_value_copy)->_link_to_resume = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_hr_id_:
                    (*initial_value_copy)->_hr_id = (*(reinterpret_cast<int *>(iter.second)));
                    break;
                case db_value_fields::_programming_language_:
                    string_holder::get_instance()->remove_string((*((*initial_value_copy)->_programming_language)));
                    (*initial_value_copy)->_programming_language = string_holder::get_instance()->get_string((*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_task_count_:
                    (*initial_value_copy)->_task_count = (*(reinterpret_cast<unsigned *>(iter.second)));
                    break;
                case db_value_fields::_solved_task_count_:
                    (*initial_value_copy)->_solved_task_count = (*(reinterpret_cast<unsigned *>(iter.second)));
                    break;
                case db_value_fields::_copying_:
                    (*initial_value_copy)->_copying = (*(reinterpret_cast<bool *>(iter.second)));
                    break;
            }
        }

        return initial_value_copy;
    }
};

class add_command final : public command
{
    std::map<db_value_fields, unsigned char *> _add_dictionary;
    // по факту, то же самое, что update command, но тут именно все поля новые, и т.к. add можно только после удаления, то нужно создать новое db_value;

public:
    explicit add_command(std::map<db_value_fields, unsigned char *> dictionary)
    : _add_dictionary(std::move(dictionary))
    {

    }

    ~add_command() override {
        for (const auto & iter : _add_dictionary) {
            delete iter.second;
        }
    }

    db_value ** execute(db_value ** initial_value_copy) override
    {
        std::cout << "add command" << std::endl;

        auto * dbValueBuilder = new db_value_builder();

        for (auto const & iter : _add_dictionary) {
            switch (iter.first) {
                case db_value_fields::_surname_:
                    dbValueBuilder->with_surname(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_name_:
                    dbValueBuilder->with_name(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_patronymic_:
                    dbValueBuilder->with_patronymic(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_birthday_:
                    dbValueBuilder->with_birthday(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_link_to_resume_:
                    dbValueBuilder->with_link_to_resume(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_hr_id_:
                    dbValueBuilder->with_hr_id((*(reinterpret_cast<int *>(iter.second))));
                    break;
                case db_value_fields::_programming_language_:
                    dbValueBuilder->with_programming_language(std::move(*(reinterpret_cast<std::string *>(iter.second))));
                    break;
                case db_value_fields::_task_count_:
                    dbValueBuilder->with_task_count((*(reinterpret_cast<unsigned *>(iter.second))));
                    break;
                case db_value_fields::_solved_task_count_:
                    dbValueBuilder->with_solved_task_count((*(reinterpret_cast<unsigned *>(iter.second))));
                    break;
                case db_value_fields::_copying_:
                    dbValueBuilder->with_copying((*(reinterpret_cast<bool *>(iter.second))));
                    break;
            }
        }

        (*initial_value_copy) = dbValueBuilder->build();
        delete dbValueBuilder;

        return initial_value_copy;
    }
};

#endif //COURSE_WORK_COMMAND_H
