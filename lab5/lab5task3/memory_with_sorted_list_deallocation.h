#ifndef MEMORY_WITH_SORTED_LIST_DEALLOCATION_H
#define MEMORY_WITH_SORTED_LIST_DEALLOCATION_H

#include "../base_classes/Memory_base_class.h"

class memory_with_sorted_list_deallocation final : public Memory
{
private:
    void * get_first_available_block_address() const override;
    size_t get_available_block_service_block_size() const override;
    size_t get_available_block_size(void * memory_block) const override;
    void * get_next_available_block_address(void * memory_block) const override;
    size_t get_occupied_block_service_block_size() const override;
    void ** get_first_available_block_address_address() const override;
    size_t get_occupied_block_size(void * const occupied_block) const override;

public:
//    Logger_impl(Logger_impl const& log) = delete;        // copying
//    Logger_impl& operator=(Logger_impl const&) = delete; // assignment
    memory_with_sorted_list_deallocation(size_t & size, Memory * parent_allocator);
    memory_with_sorted_list_deallocation(memory_with_sorted_list_deallocation const&) = delete;
    memory_with_sorted_list_deallocation& operator=(memory_with_sorted_list_deallocation const&) = delete;
    ~memory_with_sorted_list_deallocation();

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;
    void dump_occupied_block_before_deallocate(void * const current_block_address) const override;
};


#endif //MEMORY_WITH_SORTED_LIST_DEALLOCATION_H
