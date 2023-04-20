#ifndef MEMORY_WITH_BUDDY_SYSTEM_H
#define MEMORY_WITH_BUDDY_SYSTEM_H

#include "../base_classes/memory_base_class.h"
#include <cmath>

/* Structure:
 * allocator:
 *      char power
 *      logger*
 *      memory* parent_allocator
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

class memory_with_buddy_system final : public memory
{
private:
#pragma region Allocator properties
    size_t get_allocator_service_block_size() const override;
    char * _buddy_system_get_ptr_size_of_allocator_pool() const;
    logger ** _buddy_system_get_ptr_logger_of_allocator() const;
    memory ** _buddy_system_get_ptr_to_ptr_parent_allocator() const;
    void ** _buddy_system_get_ptr_to_ptr_to_pool_start() const;
    void * get_ptr_to_allocator_trusted_pool() const override;

    [[nodiscard]] logger** get_ptr_logger_of_allocator() const override;

#pragma endregion

#pragma region Buddy system block properties
    bool * _buddy_system_is_block_available(void * block) const;
    char * _buddy_system_get_size_of_block(void * block) const;
    void ** _buddy_system_get_available_block_address_field(void * block) const;
    void * get_ptr_to_buddy(void * block, void * ptr_to_pool_start) const;
#pragma endregion

#pragma region Available block methods
    void *get_first_available_block_address() const override; // get the address of the first available block in allocator

    size_t get_available_block_service_block_size() const override;
    void * get_next_available_block_address(void * memory_block) const override;
#pragma endregion

#pragma region Occupied block methods
    size_t get_occupied_block_service_block_size() const override;
    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;
#pragma endregion

    size_t get_number_in_bin_pow(char power) const;
    char get_bin_pow_of_number(size_t number) const;

public:
    memory_with_buddy_system(memory_with_buddy_system const&) = delete;
    memory_with_buddy_system& operator= (memory_with_buddy_system const&) = delete;
    memory_with_buddy_system(char pow, logger* logger, memory* parent_allocator);
    ~memory_with_buddy_system();

    void *allocate(size_t target_size) const override;

    void deallocate(void const * const target_to_dealloc) const override;
};

#endif //MEMORY_WITH_BUDDY_SYSTEM_H
