#include "memory_with_boundary_tags.h"

#pragma region Allocator properties
size_t memory_with_boundary_tags::get_allocator_service_block_size() const {
    return sizeof(size_t) + sizeof(Logger *) + sizeof(Memory *) + sizeof(void *) + sizeof(Memory::Allocation_strategy);
}
#pragma endregion

#pragma region Occupied block methods
void *memory_with_boundary_tags::get_first_occupied_block_address() const {
    return *get_ptr_to_ptr_to_pool_start();
}

void **memory_with_boundary_tags::get_first_occupied_block_address_address() const {
    return get_ptr_to_ptr_to_pool_start();
}

size_t memory_with_boundary_tags::get_occupied_block_service_block_size() const {
    return (sizeof(void *) * 2 + sizeof(size_t));
}

size_t memory_with_boundary_tags::get_occupied_block_size(void *memory_block) const {
    return *reinterpret_cast<size_t *>(memory_block);
}

size_t memory_with_boundary_tags::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return get_occupied_block_size(occupied_block) - get_occupied_block_service_block_size();
}

void *memory_with_boundary_tags::get_next_occupied_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>(reinterpret_cast<size_t *>(memory_block) + 1);
}

void *memory_with_boundary_tags::get_previous_occupied_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>((reinterpret_cast<size_t *>(memory_block) + 1) + 1);
}
#pragma endregion

memory_with_boundary_tags::memory_with_boundary_tags(size_t &size,
                                                     Memory::Allocation_strategy mode,
                                                     Logger * logger,
                                                     Memory * parent_allocator)
{
    size_t size_with_service_size = size + get_allocator_service_block_size();

    if (parent_allocator != nullptr) {
        _ptr_to_allocator_metadata = parent_allocator->allocate(size_with_service_size);
    } else {
        _ptr_to_allocator_metadata = ::operator new(size_with_service_size);
    }

    *(get_ptr_size_of_allocator_pool()) = size;
    *(get_ptr_allocation_mode()) = mode;
    *(get_ptr_logger_of_allocator()) = logger;
    *(get_ptr_to_ptr_parent_allocator()) = parent_allocator;
    *(get_ptr_to_ptr_to_pool_start()) = *reinterpret_cast<void **>(get_ptr_to_ptr_to_pool_start() + 1);

    this->log_with_guard("memory_with_boundary_tags allocator was constructed",
                         Logger::Severity::trace);
}

memory_with_boundary_tags::~memory_with_boundary_tags() {
    this->log_with_guard("memory_with_sorted_list_deallocation allocator was destructed",
                         Logger::Severity::trace);

    Memory *parent_allocator = *get_ptr_to_ptr_parent_allocator();
    if (parent_allocator) {
        parent_allocator->deallocate(this);
    }
}

/// <summary>
/// Allocates a block of memory in memory_with_boundary_tags allocator
/// </summary>
/// <param name="target_size"> - size of block to be allocated</param>
void *memory_with_boundary_tags::allocate(size_t target_size) const {
    this->log_with_guard("memory_with_boundary_tags::allocate method execution started",
                         Logger::Severity::trace);

    void *previous_block = nullptr, *current_block = get_first_available_block_address(), *target_block = nullptr;
    // TODO: redo next string
    void *start_of_allocator_space = reinterpret_cast<void *>(reinterpret_cast<char *>(_ptr_to_allocator_metadata) + get_allocator_service_block_size());
    auto const occupied_block_service_block_size = get_occupied_block_service_block_size();
    auto const allocation_mode = *get_ptr_allocation_mode();
    size_t current_block_size = 0, target_block_size = 0, size_needed = target_size + occupied_block_service_block_size;


#pragma region Finding a block of appropriate size
    // not even one block was allocated in allocator
    if (current_block == nullptr && *get_ptr_size_of_allocator_pool() >= size_needed) {
       target_block = start_of_allocator_space;
       *get_first_occupied_block_address_address() = target_block;
    }
    // at least one block was allocated
    else {
        do
        {
            // Get size of available block
            // <start> | target_block(?) | current block | ...
            if (previous_block == nullptr && current_block != start_of_allocator_space) {
                current_block_size = reinterpret_cast<char *>(current_block) - reinterpret_cast<char *>(start_of_allocator_space);
            }
            // == nullptr: ... | previous_block | target_block(?) | <end>
            // != nullptr: ... | previous_block | target_block(?) | current_block | ...
            else if (previous_block != nullptr) {
                current_block == nullptr ?
                    current_block_size = reinterpret_cast<size_t *>(start_of_allocator_space) + *get_ptr_size_of_allocator_pool()
                            - reinterpret_cast<size_t *>(previous_block) + get_occupied_block_size(previous_block)
                            :
                    current_block_size = reinterpret_cast<size_t *>(current_block)
                            - reinterpret_cast<size_t *>(previous_block) + get_occupied_block_size(previous_block);
            }

            if (current_block_size >= size_needed)
            {
                if (    allocation_mode == Memory::Allocation_strategy::first_fit ||
                        allocation_mode == Memory::Allocation_strategy::best_fit &&
                        (target_block == nullptr || current_block_size < target_block_size) ||
                        allocation_mode == Memory::Allocation_strategy::worst_fit &&
                        (target_block == nullptr || current_block_size > target_block_size))
                {
                    target_block = reinterpret_cast<size_t *>(previous_block) + *reinterpret_cast<size_t *>(previous_block);
                }

                if (allocation_mode == Memory::Allocation_strategy::first_fit)
                    break;
            }

            previous_block = current_block;
            current_block = get_next_occupied_block_address(current_block);
        } while (previous_block != nullptr);
    }
#pragma endregion

    if (target_block == nullptr)
    {
        this->log_with_guard("There is no memory available to allocate", Logger::Severity::warning)
                ->log_with_guard("memory_with_boundary_tags::allocate method execution finished", Logger::Severity::trace);

        throw std::bad_alloc();
    }

    // by allocating memory block will be divided so the leftover memory cannot include (void *) + size_t for available block structure
    if (auto leftover = target_block_size - size_needed <= occupied_block_service_block_size)
    {
        auto target_size_override = target_block_size + leftover;

        this->log_with_guard("Requested " + std::to_string(target_size) + " bytes, but reserved "
                             + std::to_string(target_size_override) + " bytes for correct work of allocator",
                             Logger::Severity::debug);

        target_size = target_size_override;
    }

#pragma region Inserting target block in list of occupied blocks
    // ... | previous_block | target_block | current_block | ...
    // target_block.prev = previous_block;
    *reinterpret_cast<void **>((reinterpret_cast<size_t *>(target_block) + 1) + 1) = previous_block;
    // target_block is not right after metadata
    if (previous_block != nullptr) {
        // target_block.prev.next = target_block;
        *reinterpret_cast<void **>(reinterpret_cast<size_t *>(previous_block) + 1) = target_block;
    } else {
        *get_first_occupied_block_address_address() = target_block;
    }

    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(target_block) + 1) = current_block;
    // target_block is not the 'last' block allocated
    if (current_block != nullptr) {
        // target_block.next.prev = target_block;
        *reinterpret_cast<void **>((reinterpret_cast<size_t *>(current_block) + 1) + 1) = target_block;
    }
#pragma endregion

    *reinterpret_cast<size_t *>(target_block) = target_size + occupied_block_service_block_size;

    this->log_with_guard("memory_with_boundary_tags::allocate method execution finished", Logger::Severity::trace);

    return *reinterpret_cast<void **>((reinterpret_cast<size_t *>(target_block) + 1) + 2);
}

void memory_with_boundary_tags::deallocate(const void *const target_to_dealloc) const {
    this->log_with_guard("memory_with_boundary_tags::deallocate method execution started", Logger::Severity::trace);

    // make target_to_dealloc to point to the beginning of the block
    *const_cast<void **>(&target_to_dealloc) = *reinterpret_cast<void **>(
                    (reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - 1)
                    - 2);

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    void *next_to_target_to_deallocate = get_next_occupied_block_address(const_cast<void *>(target_to_dealloc)),
         *prev_to_target_to_deallocate = get_previous_occupied_block_address(const_cast<void *>(target_to_dealloc));

    // the target_to_deallocate is a block which _mem_start points to
    if (prev_to_target_to_deallocate == nullptr) {
        *get_first_occupied_block_address_address() = next_to_target_to_deallocate;
    }
    // the target_to_deallocate is NOT a block which _mem_start points to
    else {
        // target_to_deallocate.prev.next = target_to_deallocate.next;
        *reinterpret_cast<void **>(reinterpret_cast<size_t *>(prev_to_target_to_deallocate) + 1) = next_to_target_to_deallocate;
    }

    // the target_to_deallocate is not the last block occupied
    if (next_to_target_to_deallocate != nullptr) {
        // target_to_deallocate.next.prev = target_to_deallocate.prev;
        *reinterpret_cast<void **>((reinterpret_cast<size_t *>(next_to_target_to_deallocate) + 1) + 1) = prev_to_target_to_deallocate;
    }

    *reinterpret_cast<void **>((reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) + 1 ) + 1) = nullptr;
    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) + 1) = nullptr;
    *reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) = 0;

    this->log_with_guard("memory_with_boundary_tags::deallocate method execution finished", Logger::Severity::trace);
}
