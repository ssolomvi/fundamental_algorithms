#include "../chain_of_resp_and_command/handler.h"

handler * db_value::get_first_handler()
{
    return this->_chain_of_resp;
}

handler * db_value::get_last_handler()
{
    return _last_handler;
}

void db_value::add_new_handler(handler * handler_)
{
    if (_chain_of_resp == nullptr) {
        _chain_of_resp = handler_;
    }

    if (_last_handler != nullptr) {
        _last_handler->set_next(handler_);
    }
    _last_handler = handler_;
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
