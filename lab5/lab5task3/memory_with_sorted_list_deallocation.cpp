#include "memory_with_sorted_list_deallocation.h"

void *memory_with_sorted_list_deallocation::allocate(size_t target_size) const {
    this->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution started",
                         Logger::Severity::trace);

    void *previous_block = nullptr, *current_block = get_first_available_block_address();
    void *target_block = nullptr, *previous_to_target_block = nullptr, *next_to_target_block = nullptr;
    auto const available_block_service_block_size = get_available_block_service_block_size();
    auto const allocation_mode = get_allocation_mode();

    while (current_block != nullptr)
    {
        auto current_block_size = get_available_block_size(current_block);
        auto next_block = get_next_available_block_address(current_block);

        if (current_block_size - available_block_service_block_size >= target_size)
        {
            if (    allocation_mode == Memory::Allocation_strategy::first_fit ||
                    allocation_mode == Memory::Allocation_strategy::best_fit &&
                    (target_block == nullptr || current_block_size < get_available_block_size(target_block)) ||
                    allocation_mode == Memory::Allocation_strategy::worst_fit &&
                    (target_block == nullptr || current_block_size > get_available_block_size(target_block)))
            {
                previous_to_target_block = previous_block;
                target_block = current_block;
                next_to_target_block = next_block;
            }

            if (allocation_mode == Memory::Allocation_strategy::first_fit) {
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
                             Logger::Severity::warning)
            ->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution failed",
                             Logger::Severity::trace);

        throw std::bad_alloc();
    }

    // by allocating memory block will be divided so the leftover memory cannot include (void *) + size_t for available block structure
    if (auto leftover = get_available_block_size(target_block) - get_occupied_block_service_block_size()
    - target_size <= get_available_block_service_block_size())
    {
        auto requested_block_size_override = target_size + leftover;

        this->log_with_guard("Requested " + std::to_string(target_size) + " bytes, but reserved "
              + std::to_string(requested_block_size_override) + " bytes for correct work of allocator",
              Logger::Severity::debug);

        target_size = requested_block_size_override;
    }

    void * updated_next_block_to_previous_block;

    // size of target block is precisely equal to available block size + void* (size_t is needed for occupied block service)
    if (target_size == get_available_block_size(target_block) - get_occupied_block_service_block_size()) {
        updated_next_block_to_previous_block = next_to_target_block;
    }
    else
    {
        updated_next_block_to_previous_block = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(target_block) +
                get_occupied_block_service_block_size() + target_size);

        auto * const target_block_leftover_size = reinterpret_cast<size_t*>(updated_next_block_to_previous_block);
        // available block size is size of full available block including service part
        *target_block_leftover_size = get_available_block_size(target_block) - get_occupied_block_service_block_size()
                - target_size;

        auto * const target_block_leftover_next_block_address = reinterpret_cast<void **>(target_block_leftover_size + 1);
        *target_block_leftover_next_block_address = next_to_target_block;
    }

    /*
     * if previous_to_target_block == nullptr, then mem_start is updated with target_block.next
     * else previous to target_block.next address is updated with target_block.next
    */
    previous_to_target_block == nullptr ?
            *get_first_available_block_address_address() = updated_next_block_to_previous_block :
            *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(previous_to_target_block) + sizeof(size_t))
            = updated_next_block_to_previous_block;

    auto *target_block_size_address = reinterpret_cast<size_t *>(target_block);
    *target_block_size_address = target_size + get_occupied_block_service_block_size();

    this->log_with_guard("memory_with_sorted_list_deallocation::allocate method execution finished",
                         Logger::Severity::trace);

    return reinterpret_cast<void *>(target_block_size_address + 1);
}

void memory_with_sorted_list_deallocation::deallocate(const void *const target_to_dealloc) const
{
    this->log_with_guard("memory_with_sorted_list_deallocation::deallocate method execution started",
                         Logger::Severity::trace);

    *const_cast<void **>(&target_to_dealloc) =
            reinterpret_cast<void *>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - sizeof(size_t));

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    void *next_to_current_block, *current_block = get_first_available_block_address();

    // current block is first block to become available
    if (current_block == nullptr) {
        // change _mem_start
        *get_first_available_block_address_address() = const_cast<void *>(target_to_dealloc);

        // make a target_to_deallocate.next = nullptr;
        *reinterpret_cast<void **>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) + 1) = nullptr;
    }

    // there is a block available previous (or before the block?) to block to be freed
    else {
        // the _mem_start is after target_to_dealloc
        if (current_block > target_to_dealloc) {
            // make a target_to_deallocate.next = _mem_start block (previously the first available block)
            *reinterpret_cast<void **>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) + 1) = current_block;
            // change _mem_start
            *get_first_available_block_address_address() = const_cast<void *>(target_to_dealloc);
        }
        // the _mem_start is before target_to_dealloc && there might be blocks before target_to_dealloc
        else {
            while (current_block != nullptr) {
                next_to_current_block = get_next_available_block_address(current_block);

                // find a position, where current block < target_to_dealloc < current_block.next ?= nullptr
                if (next_to_current_block == nullptr || next_to_current_block > target_to_dealloc) {
                    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) + 1) = next_to_current_block;

                    *reinterpret_cast<void **>(reinterpret_cast<size_t *>(current_block) + 1) = const_cast<void *>(target_to_dealloc);
                }
                current_block = next_to_current_block;
            }
        }
    }

    this->log_with_guard("memory_with_sorted_list_deallocation::deallocate method execution finished",
                         Logger::Severity::trace);
}

void memory_with_sorted_list_deallocation::dump_occupied_block_before_deallocate(void *const current_block_address) const {
    dump_occupied_block_before_deallocate_initial(current_block_address, get_occupied_block_size);
}

// returns mem_start
void *memory_with_sorted_list_deallocation::get_first_available_block_address() const
{
    return this->_mem_start;
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

size_t memory_with_sorted_list_deallocation::get_occupied_block_service_block_size() const {
    return sizeof(size_t);
}

void **memory_with_sorted_list_deallocation::get_first_available_block_address_address() const {
    return const_cast<void **>(&(this->_mem_start));
}

/*
void **memory_with_sorted_list_deallocation::get_first_available_block_address_address() const {
    return reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(get_first_available_block_address()) + sizeof(size_t));
}
*/

size_t memory_with_sorted_list_deallocation::get_occupied_block_size(void *const occupied_block) const {
    return *(reinterpret_cast<size_t *>(occupied_block) - sizeof(size_t));
}

// выделили из глобальной кучи, а не из какого-либо аллокатора
memory_with_sorted_list_deallocation::memory_with_sorted_list_deallocation(size_t & size, Memory * parent_allocator = nullptr)
{
    this->_mem_start = this + sizeof(memory_with_sorted_list_deallocation);
    this->_parent_allocator = parent_allocator;
    this->_size = size;
    this->_logger = nullptr;

    this->log_with_guard("memory_with_sorted_list_deallocation allocator was constructed",
                         Logger::Severity::trace);
}

memory_with_sorted_list_deallocation::~memory_with_sorted_list_deallocation() {
    this->log_with_guard("memory_with_sorted_list_deallocation allocator was destructed",
                         Logger::Severity::trace);

    if (this->_parent_allocator) {
        _parent_allocator->deallocate(this);
    }
}


