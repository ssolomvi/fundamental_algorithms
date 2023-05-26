#include "../chain_of_resp_and_command/handler.h"

handler * db_value::get_first_handler()
{
    return this->_chain_of_resp;
}

handler ** db_value::get_last_handler()
{
    if (_chain_of_resp == nullptr) {
        return &_chain_of_resp;
    }
    handler ** returned_by_handler_inner = _chain_of_resp->find_last_handler(_chain_of_resp);

    return (returned_by_handler_inner == nullptr
    ? &_chain_of_resp
    : returned_by_handler_inner);
}

db_value::~db_value()
{
    remove_string_from_string_holder((*_surname));
    remove_string_from_string_holder((*_name));
    remove_string_from_string_holder((*_patronymic));
    remove_string_from_string_holder((*_birthday));
    remove_string_from_string_holder((*_link_to_resume));
    remove_string_from_string_holder((*_programming_language));

    if (_chain_of_resp != nullptr) {
        _chain_of_resp->delete_chain_of_responsibility();
    }
}
