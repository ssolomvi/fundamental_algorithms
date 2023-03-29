#pragma once
#ifndef MEMORY_BASE_CLASS_H
#define MEMORY_BASE_CLASS_H

#include <functional>
#include "Logger.h"

class Memory
{
protected:
    enum Allocation_strategy {
        first_fit,
        best_fit,
        worst_fit,
    };

    Memory *_parent_allocator;
    Logger *_logger;
    size_t _size;
    void *_mem_start;
    Allocation_strategy allocationStrategy;

    void dump_occupied_block_before_deallocate_initial(void *const current_block_address,
                                                       const std::function<size_t(void * const)>& get_occupied_block_size) const;

    [[nodiscard]] Allocation_strategy get_allocation_mode() const;
    virtual void *get_first_available_block_address() const; // get the address of the first available block in allocator
    virtual size_t get_available_block_service_block_size() const;
    virtual size_t get_available_block_size(void * memory_block) const;
    virtual void * get_next_available_block_address(void * memory_block) const;
    virtual size_t get_occupied_block_service_block_size() const;   // get
    virtual void ** get_first_available_block_address_address() const;   // get an address field of the first block available in allocator
    virtual size_t get_occupied_block_size(void * const occupied_block) const = 0;

    virtual void * get_first_occupied_block_address() const;
    virtual void ** get_first_occupied_block_address_address() const;

    virtual size_t get_occupied_block_size_of_block(void * memory_block) const;
    virtual void * get_next_occupied_block_address(void * memory_block) const;
    virtual void * get_previous_occupied_block_address(void * memory_block) const;

public:
    std::string address_to_hex(void const * ptr) const;

    void set_logger(Logger * const logger);

    Memory const * log_with_guard(std::string const & target, Logger::Severity severity) const;

    virtual void *allocate(size_t target_size) const = 0;

    virtual void deallocate(void const * const target_to_dealloc) const = 0;

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

    virtual void dump_occupied_block_before_deallocate(void * const current_block_address) const = 0;
};

#endif //MEMORY_BASE_CLASS_H
