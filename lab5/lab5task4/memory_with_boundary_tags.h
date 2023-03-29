#ifndef MEMORY_WITH_BOUNDARY_TAGS_H
#define MEMORY_WITH_BOUNDARY_TAGS_H

#include "../base_classes/Memory_base_class.h"

class memory_with_boundary_tags final : public Memory
{
private:
    void * get_first_occupied_block_address() const override;
    void ** get_first_occupied_block_address_address() const override;

    size_t get_occupied_block_size_of_block(void * memory_block) const override;
    void * get_next_occupied_block_address(void * memory_block) const override;
    void * get_previous_occupied_block_address(void * memory_block) const override;

    size_t get_occupied_block_service_block_size() const override;

    size_t get_occupied_block_size(void * const occupied_block) const override;

public:
    memory_with_boundary_tags(size_t & size, Memory * parent_allocator);
    memory_with_boundary_tags(memory_with_boundary_tags const&) = delete;
    memory_with_boundary_tags& operator=(memory_with_boundary_tags const&) = delete;
    ~memory_with_boundary_tags();

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;
    void dump_occupied_block_before_deallocate(void * const current_block_address) const override;
};


#endif //MEMORY_WITH_BOUNDARY_TAGS_H
