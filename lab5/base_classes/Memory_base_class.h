#pragma once
#ifndef MEMORY_BASE_CLASS_H
#define MEMORY_BASE_CLASS_H

#include <functional>
#include "Logger.h"

class Memory
{
protected:
    enum Allocation_strategy {
        first,
        best,
        worst
    };
    Memory* _parent_allocator;
    Logger* _logger;
    size_t _size;
    void* _mem_start;

    void dump_occupied_block_before_deallocate_initial(void *const current_block_address,
                                                       const std::function<size_t(void * const)>& get_occupied_block_size) const;

public:
    std::string address_to_hex(void const * ptr) const;

    void set_logger(Logger * const logger);

    Memory const * log_with_guard(std::string const & target, Logger::Severity severity) const;

    // virtual size_t get_occupied_block_size(void * const) = 0;

    virtual void *allocate(size_t target_size) const = 0;

    friend void * const operator+=(Memory const &allocator, size_t target_size);
    friend void operator-=(Memory const &allocator, void const * const target_to_dealloc);
    /*
    void *operator+= (size_t target_size) const {
        return this->allocate(target_size);
    }

    void operator-= (void const * const target_to_dealloc) const {
        return this->deallocate(target_to_dealloc);
    }
     */

    virtual void deallocate(void const * const target_to_dealloc) const = 0;

    virtual void dump_occupied_block_before_deallocate(void * const current_block_address) = 0;
};

#endif //MEMORY_BASE_CLASS_H
