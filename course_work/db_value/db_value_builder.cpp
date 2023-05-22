#include "db_value_builder.h"

db_value_builder *db_value_builder::with_surname(std::string &&surname) {
    _surname = std::move(surname);
    return this;
}

db_value_builder *db_value_builder::with_name(std::string &&name) {
    _name = std::move(name);
    return this;
}

db_value_builder *db_value_builder::with_patronymic(std::string &&patronymic) {
    _patronymic = std::move(patronymic);
    return this;
}

db_value_builder *db_value_builder::with_birthday(std::string &&birthday) {
    _birthday = std::move(birthday);
    return this;
}

db_value_builder *db_value_builder::with_link_to_resume(std::string &&link) {
    _link_to_resume = std::move(link);
    return this;
}

db_value_builder *db_value_builder::with_hr_id(unsigned int hr_id)
{
    _hr_id = hr_id;
    return this;
}

db_value_builder *db_value_builder::with_programming_language(std::string &&programming_language) {
    _programming_language = std::move(programming_language);
    return this;
}

db_value_builder *db_value_builder::with_task_count(unsigned int task_count) {
    _task_count = task_count;
    return this;
}

db_value_builder *db_value_builder::with_solved_task_count(unsigned int solved_task_count) {
    _solved_task_count = solved_task_count;
    return this;
}

db_value_builder *db_value_builder::with_copying(bool did_copy) {
    _copying = did_copy;
    return this;
}

db_value *db_value_builder::build() const {
    return new db_value();
}
