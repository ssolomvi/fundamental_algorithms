#include "memory_with_sorted_list_deallocation.h"

#pragma region Allocator properties
size_t memory_with_sorted_list_deallocation::get_allocator_service_block_size() const {
    return sizeof(size_t) + sizeof(logger *) + sizeof(memory *) + sizeof(void *) + sizeof(memory::Allocation_strategy);
}

void * memory_with_sorted_list_deallocation::get_ptr_to_allocator_trusted_pool() const {
    return reinterpret_cast<void *>(get_ptr_to_ptr_to_pool_start() + 1);
//    return *(get_ptr_to_ptr_to_pool_start() + 1);
}

#pragma endregion

#pragma region Available block methods
void *memory_with_sorted_list_deallocation::get_first_available_block_address() const
{
    return *get_ptr_to_ptr_to_pool_start();
}

void **memory_with_sorted_list_deallocation::get_first_available_block_address_address() const {
    return get_ptr_to_ptr_to_pool_start();
}

size_t memory_with_sorted_list_deallocation::get_available_block_service_block_size() const
{
    return (sizeof(void *) + sizeof(size_t));
}

size_t memory_with_sorted_list_deallocation::get_available_block_size(void *memory_block) const
{
    return *reinterpret_cast<size_t *>(memory_block);
}

void *memory_with_sorted_list_deallocation::get_next_available_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(memory_block) + sizeof(size_t));
}
#pragma endregion

#pragma region Occupied block methods
size_t memory_with_sorted_list_deallocation::get_occupied_block_service_block_size() const {
    return sizeof(size_t);
}

size_t memory_with_sorted_list_deallocation::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return *(reinterpret_cast<size_t *>(occupied_block) - 1) - get_occupied_block_service_block_size();
}
#pragma endregion

memory_with_sorted_list_deallocation::memory_with_sorted_list_deallocation(
        size_t size,
        memory::Allocation_strategy mode,
        logger * logger,
        memory * parent_allocator)
{
    size_t size_with_service_size = size + get_allocator_service_block_size();
    size_t available_block_service_block_size = get_available_block_service_block_size();

    if (size < available_block_service_block_size) {
        size_with_service_size += available_block_service_block_size;
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

    auto * this_allocator_logger = reinterpret_cast<logger **>(size_of_allocator_pool + 1);
    *this_allocator_logger = logger;

    auto * this_allocator_parent_allocator = reinterpret_cast<memory **>(this_allocator_logger + 1);
    *this_allocator_parent_allocator = parent_allocator;

    auto * allocation_mode = reinterpret_cast<memory::Allocation_strategy *>(this_allocator_parent_allocator + 1);
    *allocation_mode = mode;

    auto * ptr_to_pool_start = reinterpret_cast<void **>(allocation_mode + 1);
    *ptr_to_pool_start = reinterpret_cast<void *>(reinterpret_cast<char *>(ptr_to_pool_start) + sizeof(void *));

    auto * first_available_block = *ptr_to_pool_start;
    auto * first_available_block_size = reinterpret_cast<size_t *>(first_available_block);
    *first_available_block_size = size;
    auto * next_to_first_available_block = reinterpret_cast<void **>(first_available_block_size + 1);
    *next_to_first_available_block = nullptr;

    this->log_with_guard("memory_with_sorted_list_deallocation allocator was constructed",
                         logger::severity::trace);
}

memory_with_sorted_list_deallocation::~memory_with_sorted_list_deallocation() {
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

void *memory_with_sorted_list_deallocation::allocate(size_t target_size) const {
    this->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution started",
                         logger::severity::trace);

    void *previous_block = nullptr, *current_block = get_first_available_block_address();
    void *target_block = nullptr, *previous_to_target_block = nullptr, *next_to_target_block = nullptr;
    auto const available_block_service_block_size = get_available_block_service_block_size();
    auto const occupied_block_service_block_size = get_occupied_block_service_block_size();
    auto const allocation_mode = *get_ptr_allocation_mode();

    while (current_block != nullptr)
    {
        auto current_block_size = get_available_block_size(current_block);
        auto next_block = get_next_available_block_address(current_block);

        if (current_block_size - occupied_block_service_block_size >= target_size)
        {
            if (allocation_mode == memory::Allocation_strategy::first_fit ||
                allocation_mode == memory::Allocation_strategy::best_fit &&
                (target_block == nullptr || current_block_size < get_available_block_size(target_block)) ||
                allocation_mode == memory::Allocation_strategy::worst_fit &&
                (target_block == nullptr || current_block_size > get_available_block_size(target_block)))
            {
                previous_to_target_block = previous_block;
                target_block = current_block;
                next_to_target_block = next_block;
            }

            if (allocation_mode == memory::Allocation_strategy::first_fit) {
                break;
            }
        }
        previous_block = current_block;
        current_block = next_block;
    }

    // if the memory block cannot be allocated
    if (target_block == nullptr)
    {
        auto const warning_message = "There is no memory available to allocate";
        this->log_with_guard(warning_message,
                             logger::severity::warning)
            ->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution failed",
                             logger::severity::trace);

        throw std::bad_alloc();
    }

    size_t leftover = get_available_block_size(target_block) - occupied_block_service_block_size - target_size;
    // by allocating memory block will be divided so the leftover memory cannot include (void *) + size_t for available block structure
    if (leftover <= available_block_service_block_size && leftover > 0)
    {
        auto requested_block_size_override = target_size + leftover;

        this->log_with_guard("Requested " + std::to_string(target_size) + " bytes, but reserved "
              + std::to_string(requested_block_size_override) + " bytes for correct work of allocator",
                             logger::severity::debug);

        target_size = requested_block_size_override;
    }

    void * updated_next_block_to_previous_block;

    // size of target block is precisely equal to available block size + void* (size_t is needed for occupied block service)
    if (leftover <= available_block_service_block_size) {
        updated_next_block_to_previous_block = next_to_target_block;
    }
    else
    {
        updated_next_block_to_previous_block = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(target_block) +
                                                                        get_occupied_block_service_block_size() + target_size);
        auto * const target_block_leftover_size = reinterpret_cast<size_t*>(updated_next_block_to_previous_block);

        // available block size is size of full available block including service part
        *target_block_leftover_size = get_available_block_size(target_block)
                                    - get_occupied_block_service_block_size() - target_size;
        auto * const target_block_leftover_next_block_address = reinterpret_cast<void **>(target_block_leftover_size + 1);

        // | target_block | leftover | available | --> merge (leftover, available)
        if (reinterpret_cast<void *>(reinterpret_cast<char *>(target_block) + get_available_block_size(target_block))
            == next_to_target_block)
        {
            *target_block_leftover_size += get_available_block_size(next_to_target_block);
            *target_block_leftover_next_block_address = get_next_available_block_address(next_to_target_block);
        }
            // | target_block | leftover | occupied |
        else {
            *target_block_leftover_next_block_address = next_to_target_block;
        }
    }

    /*
     * if previous_to_target_block == nullptr, then mem_start is updated with target_block.next
     * else previous to target_block.next address is updated with target_block.next
    */
    previous_to_target_block == nullptr ?
            *reinterpret_cast<void **>(get_first_available_block_address_address()) = updated_next_block_to_previous_block :
            *reinterpret_cast<void **>(reinterpret_cast<size_t *>(previous_to_target_block) + 1)
            = updated_next_block_to_previous_block;

    auto *target_block_size_address = reinterpret_cast<size_t *>(target_block);
    *target_block_size_address = target_size + get_occupied_block_service_block_size();

    std::string target_block_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(target_block) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())));

    this->log_with_guard("Block of size = " + std::to_string(target_size) + " was allocated. " +
                                                                                      "Address: " + target_block_address, logger::severity::information)
        ->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution finished",
                         logger::severity::trace);

    auto * to_return = reinterpret_cast<void *>(target_block_size_address + 1); 
    return to_return;
}

void memory_with_sorted_list_deallocation::deallocate(const void *const target_to_dealloc) const
{
    this->log_with_guard("memory_with_sorted_list_deallocation::deallocate method execution started",
                         logger::severity::trace);

    if (!target_to_dealloc) {
        this->log_with_guard("Target to deallocate should not be nullptr", logger::severity::warning)
                ->log_with_guard("memory_with_sorted_list_deallocation::deallocate method execution finished", logger::severity::trace);
        return;
    }

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

//    auto * tmp = reinterpret_cast<void *>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - 1);
    auto * tmp = reinterpret_cast<void *>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - 1);
//    *const_cast<void **>(&target_to_dealloc) =
//            reinterpret_cast<void *>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - sizeof(size_t));

    std::string target_to_dealloc_address = address_to_hex(reinterpret_cast<void *>(
                   reinterpret_cast<char *>(tmp) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())
           ));

    this->log_with_guard("memory block with address: " + target_to_dealloc_address + " was deallocated successfully", logger::severity::information);

    void *next_to_current_block = nullptr, *current_block = get_first_available_block_address();

    // current block is first block to become available
    if (current_block == nullptr) {
        // change _mem_start
        *get_first_available_block_address_address() = tmp;

        // make a target_to_deallocate.next = nullptr;
        *reinterpret_cast<void **>(reinterpret_cast<size_t *>(tmp) + 1) = nullptr;
    }

    // there is a block available previous (or before the block?) to block to be freed
    else {
        // the first available block is after target_to_dealloc
        if (current_block > tmp) {
            // make a target_to_deallocate.next = _mem_start block (previously the first available block)
            *reinterpret_cast<void **>(reinterpret_cast<size_t *>(tmp) + 1) = current_block;
            // change _mem_start
            *get_first_available_block_address_address() = tmp;
        }
        // the _mem_start is before target_to_dealloc && there might be blocks before target_to_dealloc
        else {
            while (current_block != nullptr) {
                next_to_current_block = get_next_available_block_address(current_block);

                // find a position, where current block < target_to_dealloc < current_block.next ?= nullptr
                if (next_to_current_block == nullptr || next_to_current_block > tmp) {
                    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(tmp) + 1) = next_to_current_block;

                    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(current_block) + 1) = tmp;
                }
                current_block = next_to_current_block;
            }
        }
    }

    this->log_with_guard("memory_with_sorted_list_deallocation::deallocate method execution finished",
                         logger::severity::trace);
}
