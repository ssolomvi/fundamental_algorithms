#include "memory_with_boundary_tags.h"

#pragma region Allocator properties
size_t memory_with_boundary_tags::get_allocator_service_block_size() const {
    return sizeof(size_t) + sizeof(logger *) + sizeof(memory *) + sizeof(void *) + sizeof(memory::Allocation_strategy);
}

void * memory_with_boundary_tags::get_ptr_to_allocator_trusted_pool() const {
    return reinterpret_cast<void *>(get_ptr_to_ptr_to_pool_start() + 1);
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
    return get_occupied_block_size(reinterpret_cast<size_t *>(reinterpret_cast<void **>(occupied_block) - 2) - 1)
         - get_occupied_block_service_block_size();
}

void *memory_with_boundary_tags::get_next_occupied_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>(reinterpret_cast<size_t *>(memory_block) + 1);
}

void *memory_with_boundary_tags::get_previous_occupied_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>((reinterpret_cast<size_t *>(memory_block) + 1) + 1);
}
#pragma endregion

memory_with_boundary_tags::memory_with_boundary_tags(size_t size,
                                                     memory::Allocation_strategy mode,
                                                     logger * logger,
                                                     memory * parent_allocator)
{
    size_t size_with_service_size = size + get_allocator_service_block_size(),
    occupied_block_service_block_size = get_occupied_block_service_block_size();

    if (size <= occupied_block_service_block_size) {
        size_with_service_size += occupied_block_service_block_size;
        this->log_with_guard("Requested " + std::to_string(size) + " bytes, but reserved "
                             + std::to_string(size_with_service_size) + " bytes for correct work of allocator",
                             logger::severity::debug);
    }

    if (parent_allocator != nullptr) {
        _ptr_to_allocator_metadata = parent_allocator->allocate(size_with_service_size);
    } else {
        _ptr_to_allocator_metadata = ::operator new(size_with_service_size);
    }

    auto * size_of_allocator_pool = reinterpret_cast<size_t *>(_ptr_to_allocator_metadata);
    *size_of_allocator_pool = size;

    auto * this_allocator_logger = reinterpret_cast<class logger **>(size_of_allocator_pool + 1);
    *this_allocator_logger = logger;

    auto * this_allocator_parent_allocator = reinterpret_cast<memory **>(this_allocator_logger + 1);
    *this_allocator_parent_allocator = parent_allocator;

    auto * allocation_mode = reinterpret_cast<memory::Allocation_strategy *>(this_allocator_parent_allocator + 1);
    *allocation_mode = mode;

    auto * ptr_to_pool_start = reinterpret_cast<void **>(allocation_mode + 1);
    *ptr_to_pool_start = nullptr;

    this->log_with_guard("memory_with_boundary_tags allocator was constructed",
                         logger::severity::trace);
}

memory_with_boundary_tags::~memory_with_boundary_tags() {
    this->log_with_guard("memory_with_sorted_list_deallocation allocator was destructed",
                         logger::severity::trace);

    auto * size_of_allocator_pool = reinterpret_cast<size_t *>(_ptr_to_allocator_metadata);
    auto * this_allocator_logger = reinterpret_cast<logger **>(size_of_allocator_pool + 1);
    auto * this_allocator_parent_allocator = *reinterpret_cast<memory **>(this_allocator_logger + 1);

    if (this_allocator_parent_allocator) {
        this_allocator_parent_allocator->deallocate(_ptr_to_allocator_metadata);
    } else {
        ::operator delete(_ptr_to_allocator_metadata);
    }
}

/// <summary>
/// Allocates a block of memory in memory_with_boundary_tags allocator
/// </summary>
/// <param name="target_size"> - size of block to be allocated</param>
void *memory_with_boundary_tags::allocate(size_t target_size) const {
    this->log_with_guard("memory_with_boundary_tags::allocate method execution started",
                         logger::severity::trace);

    size_t size_of_allocator_pool = *get_ptr_size_of_allocator_pool();
    void *previous_block = nullptr, *current_block = *get_ptr_to_ptr_to_pool_start(), *target_block = nullptr;
    void * next_to_target_block = nullptr, * prev_to_target_block = nullptr;

    void *start_of_allocator_pool = get_ptr_to_allocator_trusted_pool(),
    * end_of_allocator_pool = reinterpret_cast<void *>(reinterpret_cast<char *>(start_of_allocator_pool) + size_of_allocator_pool);
    auto const occupied_block_service_block_size = get_occupied_block_service_block_size();
    auto const allocation_mode = *get_ptr_allocation_mode();
    size_t probable_target_block_size = 0, target_block_size = 0,
           size_needed = target_size + occupied_block_service_block_size;


#pragma region Finding a block of appropriate size
    // not even one block was allocated in allocator
    if (current_block == nullptr && size_of_allocator_pool >= size_needed) {
       target_block = start_of_allocator_pool;
//       *get_first_occupied_block_address_address() = target_block;
    }
    // at least one block was allocated
    else {
        do {
            // Get size of available block
            // <start> | target_block(?) | current block | ...
            if (previous_block == nullptr && current_block != start_of_allocator_pool) {
                probable_target_block_size = reinterpret_cast<char *>(current_block) - reinterpret_cast<char *>(start_of_allocator_pool);
            }
            // == nullptr: ... | previous_block | target_block(?) | <end>
            // != nullptr: ... | previous_block | target_block(?) | current_block | ...
            else if (previous_block != nullptr) {
                current_block == nullptr ?
                    probable_target_block_size = reinterpret_cast<char *>(end_of_allocator_pool)
                            - (reinterpret_cast<char *>(previous_block) + get_occupied_block_size(previous_block))
                            :
                    probable_target_block_size = reinterpret_cast<char *>(current_block)
                            - (reinterpret_cast<char *>(previous_block) + get_occupied_block_size(previous_block));
            }

            if (probable_target_block_size >= size_needed)
            {
                if (allocation_mode == memory::Allocation_strategy::first_fit ||
                    allocation_mode == memory::Allocation_strategy::best_fit &&
                    (target_block == nullptr || probable_target_block_size < target_block_size || target_block_size == 0) ||
                    allocation_mode == memory::Allocation_strategy::worst_fit &&
                    (target_block == nullptr || probable_target_block_size > target_block_size || target_block_size == 0))
                {
                    if (previous_block) {
                        target_block = reinterpret_cast<void *>(reinterpret_cast<char *>(previous_block) +
                                                                get_occupied_block_size(previous_block));
                    } else {
                        target_block = start_of_allocator_pool;
                    }
                    target_block_size = probable_target_block_size;
                    next_to_target_block = current_block;
                    prev_to_target_block = previous_block;
                }

                if (allocation_mode == memory::Allocation_strategy::first_fit)
                    break;
            }

            if (current_block != nullptr) {
                previous_block = current_block;
                current_block = get_next_occupied_block_address(current_block);
            } else {
                break;
            }
        } while (previous_block != nullptr);
    }
#pragma endregion

    if (target_block == nullptr) {
        this->log_with_guard("There is no memory available to allocate", logger::severity::warning)
                ->log_with_guard("memory_with_boundary_tags::allocate method execution finished", logger::severity::trace);

        throw std::bad_alloc();
    }

    size_t leftover = 0;
    // | prev | target | current | ...
    // | metadata | target | current | ...
    if (next_to_target_block) {
        leftover = reinterpret_cast<char *>(next_to_target_block) - (reinterpret_cast<char *>(target_block) + size_needed);
    }
    // | previous | target | -> nullptr
    if (!next_to_target_block && prev_to_target_block) {
        leftover = reinterpret_cast<char *>(end_of_allocator_pool) - (reinterpret_cast<char *>(target_block) + size_needed);
    }

    // by allocating memory block will be divided so the leftover memory cannot include (void *) + size_t for available block structure
    if (leftover <= occupied_block_service_block_size && leftover > 0)
    {
        auto target_size_override = target_size + leftover;

        this->log_with_guard("Requested " + std::to_string(target_size) + " bytes, but reserved "
                             + std::to_string(target_size_override) + " bytes for correct work of allocator",
                             logger::severity::debug);

        size_needed += leftover;
    }

#pragma region Inserting target block in list of occupied blocks
    // ... | previous_block | target_block | current_block | ...
    // target_block.prev = previous_block;
    *reinterpret_cast<void **>((reinterpret_cast<size_t *>(target_block) + 1) + 1) = prev_to_target_block;
    // target_block is not right after metadata
    if (prev_to_target_block != nullptr) {
        // target_block.prev.next = target_block;
        *reinterpret_cast<void **>(reinterpret_cast<size_t *>(prev_to_target_block) + 1) = target_block;
    } else {
        *get_first_occupied_block_address_address() = target_block;
    }

    // target_block.next = current_block
    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(target_block) + 1) = next_to_target_block;
    // target_block is not the 'last' block allocated
    if (next_to_target_block != nullptr) {
        // target_block.next.prev = target_block;
        *reinterpret_cast<void **>((reinterpret_cast<size_t *>(next_to_target_block) + 1) + 1) = target_block;
    }
#pragma endregion

    *reinterpret_cast<size_t *>(target_block) = size_needed;

    std::string target_block_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(target_block) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())
            ));

    this->log_with_guard("Block of size = " + std::to_string(target_size) + " was allocated. " +
                         "Address: " + target_block_address, logger::severity::information)
            ->log_with_guard("memory_with_boundary_tags::allocate method execution finished", logger::severity::trace);

    void * to_return = reinterpret_cast<void *>(reinterpret_cast<void **>(reinterpret_cast<size_t *>(target_block) + 1) + 2);
    return to_return;
}

void memory_with_boundary_tags::deallocate(const void *const target_to_dealloc) const {
    this->log_with_guard("memory_with_boundary_tags::deallocate method execution started",
                         logger::severity::trace);

    if (!target_to_dealloc) {
        this->log_with_guard("Target to deallocate should not be nullptr", logger::severity::warning)
                ->log_with_guard("memory_with_boundary_tags::deallocate method execution finished", logger::severity::trace);
        return;
    }

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    // make target_to_dealloc to point to the beginning of the block
    auto * tmp = reinterpret_cast<void *>(reinterpret_cast<size_t *>(
            reinterpret_cast<void **>(const_cast<void *>(target_to_dealloc)) - 2) - 1);

    std::string target_to_dealloc_address = address_to_hex(reinterpret_cast<void *>(
                   reinterpret_cast<char *>(tmp) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())
           ));

    this->log_with_guard("memory block with address: " + target_to_dealloc_address + " was deallocated successfully",
                         logger::severity::information);

    void *next_to_target_to_deallocate = get_next_occupied_block_address(tmp),
         *prev_to_target_to_deallocate = get_previous_occupied_block_address(tmp);

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

    *reinterpret_cast<void **>((reinterpret_cast<size_t *>(tmp) + 1 ) + 1) = nullptr;
    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(tmp) + 1) = nullptr;
    *reinterpret_cast<size_t *>(tmp) = 0;

    this->log_with_guard("memory_with_boundary_tags::deallocate method execution finished", logger::severity::trace);
}
