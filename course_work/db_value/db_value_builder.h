#ifndef DB_VALUE_BUILDER_H
#define DB_VALUE_BUILDER_H

#include "db_value.h"
#include <sstream>

class db_value_builder {
    std::string _surname, _name, _patronymic;
    std::string _birthday;
    std::string _link_to_resume;
    int _hr_id;
    std::string _programming_language;
    unsigned _task_count;
    unsigned _solved_task_count;
    bool _copying;

public:
    db_value_builder * with_surname(std::string && surname);
    db_value_builder * with_name(std::string && name);
    db_value_builder * with_patronymic(std::string && patronymic);
    db_value_builder * with_birthday(std::string && birthday);
    db_value_builder * with_link_to_resume(std::string && link);
    db_value_builder * with_hr_id(int hr_id);
    db_value_builder * with_programming_language(std::string && programming_language);
    db_value_builder * with_task_count(unsigned task_count);
    db_value_builder * with_solved_task_count(unsigned solved_task_count);
    db_value_builder * with_copying(bool did_copy);

    [[nodiscard]] db_value *build() const;

private:
    static void check_if_string_of_value_is_empty(std::string const& s)
    {
        if (s.empty()) {
            throw db_value::create_exception("db_user_communication:: incorrect input while building a value");
        }
    }

public:
    db_value *build_from_stream(std::stringstream *input_stream, bool is_cin);

};

#endif //DB_VALUE_BUILDER_H
