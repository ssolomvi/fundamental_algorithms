#include "memory_from_global_heap.h"

void *memory_from_global_heap::allocate(size_t target_size) const {
    this->log_with_guard("memory_from_global_heap::allocate method execution started", Logger::Severity::trace);

    try
    {
        auto *block_pointer = ::operator new(target_size + sizeof(size_t));
        *reinterpret_cast<size_t *>(block_pointer) = target_size;

//        memory_from_global_heap::_allocated_blocks.insert(std::make_pair(block_pointer, target_size));

        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + " was allocated successfully",
                             Logger::Severity::information)
            ->log_with_guard("Allocated block address: " + address_to_hex(block_pointer),
                             Logger::Severity::debug)
            ->log_with_guard("memory_from_global_heap::allocate method execution finished",
                             Logger::Severity::trace);

        return reinterpret_cast<void *>(reinterpret_cast<size_t *>(block_pointer) + 1);
    }
    catch (std::bad_alloc const &) {
        this->log_with_guard("Memory block with _size = " + std::to_string(target_size) + " cannot be allocated",
                             Logger::Severity::error)
            ->log_with_guard("memory_from_global_heap::allocate method execution finished",
                             Logger::Severity::trace);

        throw;
    }
}

void memory_from_global_heap::deallocate(const void *const target_to_dealloc) const {
    this->log_with_guard("memory_from_global_heap::deallocate method execution started",
                         Logger::Severity::trace);

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));
    void * tmp = reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - 1;

    std::string address = address_to_hex(tmp);
//    memory_from_global_heap::_allocated_blocks.erase(const_cast<void *>(target_to_dealloc));

    ::operator delete(tmp);

    this->log_with_guard("Memory block with address: " + address + " was deallocated successfully",
                         Logger::Severity::information)
            ->log_with_guard("memory_from_global_heap::deallocate method execution finished",
                         Logger::Severity::trace);
}

// ptr to metadata start
size_t memory_from_global_heap::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return *(reinterpret_cast<size_t *>(occupied_block) - 1);
}

memory_from_global_heap::memory_from_global_heap() {
    // TODO: throw an error message if not allocated
    _ptr_to_allocator_metadata = ::operator new(sizeof(Logger *));
}

memory_from_global_heap::~memory_from_global_heap() {
    ::operator delete(_ptr_to_allocator_metadata);
}

Logger **memory_from_global_heap::get_ptr_logger_of_allocator() const {
    return reinterpret_cast<Logger **>(_ptr_to_allocator_metadata);
}
