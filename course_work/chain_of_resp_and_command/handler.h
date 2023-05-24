#ifndef HANDLER_H
#define HANDLER_H

#include "../db_value/db_value.h"
#include "command.h"

class db_value;

class handler {
public:
    virtual handler *set_next(handler *handler) = 0;
    
    virtual db_value* handle(db_value * value, tm * time_parameter) = 0;
};

class abstract_handler : public handler
{
private:
    handler * _next_handler;

protected:
    db_value * _initial_value_copy;
    tm * timestamp;

public:
    abstract_handler(db_value * initial_value, time_t now)
    : _next_handler(nullptr)
    {
        // todo: do a copy of db_value initial
        timestamp = gmtime(&now);
        _initial_value_copy = initial_value->make_a_copy();
    }

    handler *set_next(handler *handler) override
    {
        this->_next_handler = handler;

        return handler;
    }

    db_value* handle(db_value * request, tm * time_parameter) override
    {
        if (this->_next_handler && this->_next_handler.) {
            return this->_next_handler->handle(request);
        }

        return {};
    }
};

#endif //HANDLER_H
