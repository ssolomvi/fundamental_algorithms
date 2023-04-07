#include "memory_from_global_heap.h"

void *memory_from_global_heap::allocate(size_t target_size) const {
    this->log_with_guard("memory_from_global_heap:: allocate method execution started", Logger::Severity::trace);

    try
    {
        auto *block_pointer = ::operator new(target_size);

        memory_from_global_heap::_allocated_blocks.insert(std::make_pair(block_pointer, target_size));

        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + "was allocated successfully",
                             Logger::Severity::information)
            ->log_with_guard("Allocated block address: " + address_to_hex(block_pointer),
                             Logger::Severity::debug)
            ->log_with_guard("memory_from_global_heap:: allocate method execution started",
                             Logger::Severity::debug);

        return block_pointer;
    }
    catch (std::bad_alloc const &) {
        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + "cannot be allocated",
                             Logger::Severity::error)
            ->log_with_guard("memory_from_global_heap:: allocate method execution started",
                             Logger::Severity::trace);

        throw;
    }
}

void memory_from_global_heap::deallocate(const void *const target_to_dealloc) const {
    this->log_with_guard("memory_from_global_heap::deallocate method execution started",
                         Logger::Severity::trace);

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    std::string address = address_to_hex(target_to_dealloc);
    memory_from_global_heap::_allocated_blocks.erase(const_cast<void *>(target_to_dealloc));

    ::operator delete(const_cast<void*>(target_to_dealloc));

    this->log_with_guard("Memory block with address: " + address + "was deallocated successfully",
                         Logger::Severity::information)
            ->log_with_guard("memory_from_global_heap::deallocate method execution finished",
                         Logger::Severity::trace);
}

size_t memory_from_global_heap::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return memory_from_global_heap::_allocated_blocks.find(occupied_block)->second;
}