#ifndef MEMORY_WITH_BUDDY_SYSTEM_H
#define MEMORY_WITH_BUDDY_SYSTEM_H

#include "../base_classes/Memory_base_class.h"
#include <cmath>

/* Structure:
 * allocator:
 *      unsigned short power
 *      Logger*
 *      Memory* parent_allocator
 *      void * pool start
 *
 * available_block:
 *      bool is_available
 *      size_t power
 *      void * ptr to next
 *
 * occupied block:
 *      bool is_available
 *      size_t power
 * */

class memory_with_buddy_system final : public Memory
{
private:
    unsigned short * _buddy_system_get_ptr_size_of_allocator_pool() const;
    Logger ** _buddy_system_get_ptr_logger_of_allocator() const;
    Memory ** _buddy_system_get_ptr_to_ptr_parent_allocator() const;
    void ** _buddy_system_get_ptr_to_ptr_to_pool_start() const;

    bool * _buddy_system_is_block_available(void * block) const;
    unsigned short * _buddy_system_get_size_of_block(void * block) const;
    void ** _buddy_system_get_available_block_address_field(void * block) const;


    size_t get_allocator_service_block_size() const override;

    void *get_first_available_block_address() const override; // get the address of the first available block in allocator
    size_t get_available_block_service_block_size() const override;
    void * get_next_available_block_address(void * memory_block) const override;

    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;

    size_t get_occupied_block_service_block_size() const override;   // get
    void ** get_first_available_block_address_address() const override;   // get an address field of the first block available in allocator

    size_t get_number_in_bin_pow(size_t power) const;
    unsigned short get_bin_pow_of_number(size_t number) const;

    void ** get_ptr_to_buddy(void * block) const;
public:
    memory_with_buddy_system(memory_with_buddy_system const&) = delete;
    memory_with_buddy_system& operator= (memory_with_buddy_system const&) = delete;
    memory_with_buddy_system(unsigned short pow, Logger* logger, Memory* parent_allocator);
    ~memory_with_buddy_system();

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;

};


#endif //MEMORY_WITH_BUDDY_SYSTEM_H
