#ifndef MEMORY_WITH_SORTED_LIST_DEALLOCATION_H
#define MEMORY_WITH_SORTED_LIST_DEALLOCATION_H

#include "../base_classes/memory_base_class.h"

/* Structure of memory_with_sorted_list_deallocation allocator:
 *      basic
 *
 * Structure of available blocks:
 *      size_t size of block including service part
 *      void * ptr to next available block
 *
 * Structure of occupied blocks:
 *      size_t size of occupied block including service part
 *
 * */

class memory_with_sorted_list_deallocation final : public memory
{
private:
#pragma region Allocator properties
    size_t get_allocator_service_block_size() const override;
    void * get_ptr_to_allocator_trusted_pool() const override;
#pragma endregion

#pragma region Available block methods
    void * get_first_available_block_address() const override;
    void ** get_first_available_block_address_address() const override;

    size_t get_available_block_service_block_size() const override;
    size_t get_available_block_size(void * memory_block) const override;
    void * get_next_available_block_address(void * memory_block) const override;

#pragma endregion

#pragma region Occupied block methods

    size_t get_occupied_block_service_block_size() const override;
    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;

#pragma endregion

public:
    memory_with_sorted_list_deallocation(memory_with_sorted_list_deallocation const&) = delete;            // copying
    memory_with_sorted_list_deallocation& operator=(memory_with_sorted_list_deallocation const&) = delete; // assignment

    memory_with_sorted_list_deallocation(size_t size, memory::Allocation_strategy mode, logger*, memory *);
    ~memory_with_sorted_list_deallocation();

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;

private:
    logger *get_logger() const noexcept override;
};


#endif //MEMORY_WITH_SORTED_LIST_DEALLOCATION_H
