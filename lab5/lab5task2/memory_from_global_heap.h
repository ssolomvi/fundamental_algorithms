#ifndef MEMORY_FROM_GLOBAL_HEAP_H
#define MEMORY_FROM_GLOBAL_HEAP_H

#include "../base_classes/memory_base_class.h"


class memory_from_global_heap final : public memory
{
//protected:
//    static std::map< void *, size_t > _allocated_blocks;
//    std::map< void *, size_t > _allocated_blocks;
    logger** get_ptr_logger_of_allocator() const override;
public:
    memory_from_global_heap();
    ~memory_from_global_heap();
    memory_from_global_heap(memory_from_global_heap const &) = delete;
    memory_from_global_heap& operator=(memory_from_global_heap const&) = delete;

    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;

    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;
    // static size_t get_size_of_occupied_block_pool(void * const ptr_to_block);
};

//std::map< void *, size_t > memory_from_global_heap::_allocated_blocks = std::map< void *, size_t > ();

#endif //MEMORY_FROM_GLOBAL_HEAP_H
