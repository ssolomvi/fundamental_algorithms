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

db_value_builder *db_value_builder::with_hr_id(int hr_id)
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

// todo: check if it works
db_value* db_value_builder::build() const {
    return new db_value(_surname, _name, _patronymic,
                    _birthday,
                    _link_to_resume,
                    _hr_id,
                    _programming_language,
                    _task_count, _solved_task_count,
                    _copying);
}

db_value* db_value_builder::build_from_stream(std::istringstream *input_stream, bool is_cin) {
    std::string token, delimiter = " ";
    size_t pos;
    unsigned delimiter_length = delimiter.length();

    if (is_cin) {
        std::cout << "Enter value:" << std::endl;
        std::cout << "Surname Name Patronymic:              >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }

    if ((pos = token.find(delimiter)) != std::string::npos) {
        this->with_surname(std::move(token.substr(0, pos)));
        token.erase(0, pos + delimiter_length);
    } else {
        throw db_value::create_exception("Incorrect input while building a value");
    }

    if ((pos = token.find(delimiter)) != std::string::npos) {
        this->with_name(std::move(token.substr(0, pos)));
        token.erase(0, pos + delimiter_length);
    } else {
        throw db_value::create_exception("Incorrect input while building a value");
    }

    if (token.empty()) {
        throw db_value::create_exception("Incorrect input while building a value");
    }
    this->with_patronymic(std::move(token));

    if (is_cin) {
        std::cout << "Birthday <dd/mm/year>:                >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_birthday(std::move(token));

    if (is_cin) {
        std::cout << "Link to resume:                       >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_link_to_resume(std::move(token));

    if (is_cin) {
        std::cout << "id of HR-manager:                     >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_hr_id(stoi(token));

    if (is_cin) {
        std::cout << "Programming language:                 >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_programming_language(std::move(token));

    if (is_cin) {
        std::cout << "Task count: {whole number}            >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_task_count(stoi(token));

    if (is_cin) {
        std::cout << "Solved tasks count {whole number}:    >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_solved_task_count(stoi(token));

    if (is_cin) {
        std::cout << "Copying with a micro-earphone {bool}: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);

    if (token == "true" || token == "1") {
        this->with_copying(true);
    } else if (token == "false" || token == "0") {
        this->with_copying(false);
    } else {
        throw db_value::create_exception("Incorrect input while building a value {bool must be true/false || 1/0}");
    }
    return (this->build());
}
