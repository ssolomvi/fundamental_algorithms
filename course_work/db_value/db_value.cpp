#include "../chain_of_resp_and_command/handler.h"

handler ** db_value::get_last_handler()
{
    if (chain_of_resp == nullptr) {
        return &chain_of_resp;
    }
    handler ** returned_by_handler_inner = chain_of_resp->find_last_handler(chain_of_resp);

    return (returned_by_handler_inner == nullptr
    ? &chain_of_resp
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

    chain_of_resp->delete_chain_of_responsibility();
}
