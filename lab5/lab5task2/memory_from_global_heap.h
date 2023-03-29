#pragma once
#ifndef MEMORY_FROM_GLOBAL_HEAP_H
#define MEMORY_FROM_GLOBAL_HEAP_H

#include "../base_classes/Memory_base_class.h"


class memory_from_global_heap final : public Memory
{
protected:
    static std::map< void *, size_t > _allocated_blocks;

public:
    void *allocate(size_t target_size) const override;
    void deallocate(void const * const target_to_dealloc) const override;

    size_t get_occupied_block_size(void * const occupied_block) const override;
    // static size_t get_occupied_block_size(void * const ptr_to_block);
    void dump_occupied_block_before_deallocate(void * const current_block_address) const override;
};

std::map< void *, size_t > memory_from_global_heap::_allocated_blocks = std::map< void *, size_t > ();

void *memory_from_global_heap::allocate(size_t target_size) const {
    this->log_with_guard("memory_from_global_heap:: allocate method execution started", Logger::Severity::trace);

    try
    {
        auto *block_pointer = ::operator new(target_size);

        memory_from_global_heap::_allocated_blocks.insert(std::make_pair(block_pointer, target_size));

        // if _allocated_blocks is not static
        // No matching member function for call to 'insert'
        // this->_allocated_blocks.insert(std::make_pair((void * )block_pointer, target_size));

        // Expected unqualified-id
        // this->const_cast<std::map<void*, size_t>>(_allocated_blocks).insert(std::make_pair(block_pointer, target_size));

        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + "was allocated successfully", Logger::Severity::information)
            ->log_with_guard("Allocated block address: " + address_to_hex(block_pointer), Logger::Severity::debug)
            ->log_with_guard("memory_from_global_heap:: allocate method execution started", Logger::Severity::debug);

        return block_pointer;
    }
    catch (std::bad_alloc const &) {
        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + "cannot be allocated", Logger::Severity::error)
            ->log_with_guard("memory_from_global_heap:: allocate method execution started", Logger::Severity::trace);

        throw;
    }
    return nullptr;
}

void memory_from_global_heap::deallocate(const void *const target_to_dealloc) const {
    this->log_with_guard("memory_from_global_heap::deallocate method execution started", Logger::Severity::trace);

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    std::string address = address_to_hex(target_to_dealloc);
    memory_from_global_heap::_allocated_blocks.erase(const_cast<void *>(target_to_dealloc));

    ::operator delete(const_cast<void*>(target_to_dealloc));

    this->log_with_guard("Memory block with address: " + address + "was deallocated successfully", Logger::Severity::information)
        ->log_with_guard("memory_from_global_heap::deallocate method execution finished", Logger::Severity::trace);
}

size_t memory_from_global_heap::get_occupied_block_size(void *const occupied_block) const {
    return memory_from_global_heap::_allocated_blocks.find(occupied_block)->second;
}

void memory_from_global_heap::dump_occupied_block_before_deallocate(void *const current_block_address) const {
    dump_occupied_block_before_deallocate_initial(current_block_address, get_occupied_block_size);
}



#endif //MEMORY_FROM_GLOBAL_HEAP_H
