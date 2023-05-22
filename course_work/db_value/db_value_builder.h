#ifndef COURSE_WORK_DB_VALUE_BUILDER_H
#define COURSE_WORK_DB_VALUE_BUILDER_H

#include "db_value.h"

class db_value_builder {
    std::string _surname, _name, _patronymic;
    std::string _birthday;
    std::string _link_to_resume;
    unsigned _hr_id;
    std::string _programming_language;
    unsigned _task_count;
    unsigned _solved_task_count;
    bool _copying;

    db_value_builder * with_surname(std::string && surname);
    db_value_builder * with_name(std::string && name);
    db_value_builder * with_patronymic(std::string && patronymic);
    db_value_builder * with_birthday(std::string && birthday);
    db_value_builder * with_link_to_resume(std::string && link);
    db_value_builder * with_hr_id(unsigned hr_id);
    db_value_builder * with_programming_language(std::string && programming_language);
    db_value_builder * with_task_count(unsigned task_count);
    db_value_builder * with_solved_task_count(unsigned solved_task_count);
    db_value_builder * with_copying(bool did_copy);

    db_value * build() const;
};


#endif //COURSE_WORK_DB_VALUE_BUILDER_H
