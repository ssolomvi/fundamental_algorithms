#ifndef MEMORY_WITH_BOUNDARY_TAGS_H
#define MEMORY_WITH_BOUNDARY_TAGS_H

#include "../allocator/memory_base_class.h"

/* structure of occupied memory block:
        size_t size;
        void *next, *prev;
   available blocks do not store any metadata
 */

class memory_with_boundary_tags final : public memory
{
private:
#pragma region Allocator properties
    size_t get_allocator_service_block_size() const override;
    void * get_ptr_to_allocator_trusted_pool() const override;
#pragma endregion

#pragma region Occupied block methods
    void ** get_first_occupied_block_address_address() const override;

    size_t get_occupied_block_service_block_size() const override;
    size_t get_occupied_block_size(void * memory_block) const override;
    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;

    void * get_next_occupied_block_address(void * memory_block) const override;
    void * get_previous_occupied_block_address(void * memory_block) const override;
#pragma endregion

public:
    memory_with_boundary_tags(size_t size, memory::Allocation_strategy mode, logger* logger, memory * parent_allocator);
    ~memory_with_boundary_tags();

    memory_with_boundary_tags(memory_with_boundary_tags const&) = delete;
    memory_with_boundary_tags& operator=(memory_with_boundary_tags const&) = delete;

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;

private:
    logger *get_logger() const noexcept override;
};

#endif //MEMORY_WITH_BOUNDARY_TAGS_H
