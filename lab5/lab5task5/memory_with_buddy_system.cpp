#include "memory_with_buddy_system.h"

#pragma region Allocator properties
size_t memory_with_buddy_system::get_allocator_service_block_size() const {
    return sizeof(char) + sizeof(logger *) + sizeof(memory *) + sizeof(void *);
}

char *memory_with_buddy_system::_buddy_system_get_ptr_size_of_allocator_pool() const {
    return reinterpret_cast<char *>(_ptr_to_allocator_metadata);
}

logger **memory_with_buddy_system::_buddy_system_get_ptr_logger_of_allocator() const {
    return reinterpret_cast<logger **>(_buddy_system_get_ptr_size_of_allocator_pool() + 1);
}

memory **memory_with_buddy_system::_buddy_system_get_ptr_to_ptr_parent_allocator() const {
    return reinterpret_cast<memory **>(_buddy_system_get_ptr_logger_of_allocator() + 1);
}

void **memory_with_buddy_system::_buddy_system_get_ptr_to_ptr_to_pool_start() const {
    return reinterpret_cast<void **>(_buddy_system_get_ptr_to_ptr_parent_allocator() + 1);
}

void *memory_with_buddy_system::get_ptr_to_allocator_trusted_pool() const {
    return reinterpret_cast<void *>(_buddy_system_get_ptr_to_ptr_to_pool_start() + 1);
}

logger **memory_with_buddy_system::get_ptr_logger_of_allocator() const {
    return _buddy_system_get_ptr_logger_of_allocator();
}
#pragma endregion

#pragma region Buddy system block properties
bool * memory_with_buddy_system::_buddy_system_is_block_available(void *block) const {
    return reinterpret_cast<bool *>(block);
}

char * memory_with_buddy_system::_buddy_system_get_size_of_block(void *block) const {
    return reinterpret_cast<char *>(_buddy_system_is_block_available(block) + 1);
}

void **memory_with_buddy_system::_buddy_system_get_available_block_address_field(void * block) const {
    return reinterpret_cast<void **>(_buddy_system_get_size_of_block(block) + 1);
}

// [ buddy   ][ our block ]
void *memory_with_buddy_system::get_ptr_to_buddy(void * block, void * ptr_to_pool_start) const {
    auto * block_relatively_pool = reinterpret_cast<void *>(reinterpret_cast<char *>(block) - reinterpret_cast<char *>(ptr_to_pool_start));

    size_t shift =
            reinterpret_cast<size_t>(block_relatively_pool) ^ get_number_in_bin_pow(*_buddy_system_get_size_of_block(block));

    auto * to_return = reinterpret_cast<void *>(shift + reinterpret_cast<char *>(ptr_to_pool_start));
//    auto * to_return = reinterpret_cast<void *>(
//            reinterpret_cast<size_t>(block) ^ get_number_in_bin_pow((*_buddy_system_get_size_of_block(block))));
    return to_return;
}
// ini 0x1bee6b45319
// 11: 0x1bee6b45b19
// 10: 0x1bee6b45719
// 9:  0x1bee6b45119 < initial
// 8:  0x1bee6b45219 < initial
// 7:  0x1bee6b45399
// 6:  0x1bee6b45359

#pragma endregion

#pragma region Available block methods
void *memory_with_buddy_system::get_first_available_block_address() const {
    return *_buddy_system_get_ptr_to_ptr_to_pool_start();
}

size_t memory_with_buddy_system::get_available_block_service_block_size() const {
    return sizeof(bool) + sizeof(char) + sizeof(void *);
}

void *memory_with_buddy_system::get_next_available_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>(
            reinterpret_cast<char *>(
                    reinterpret_cast<bool *>(memory_block) + 1) + 1);
}
#pragma endregion

#pragma region Occupied block methods
size_t memory_with_buddy_system::get_occupied_block_service_block_size() const {
    return sizeof(bool) + sizeof(char);
}

size_t memory_with_buddy_system::get_size_of_occupied_block_pool(void *const occupied_block) const {
    void * beginning_of_block = reinterpret_cast<void *>(reinterpret_cast<bool *>(
            reinterpret_cast<char *>(occupied_block) - 1) - 1);
    return get_number_in_bin_pow(*_buddy_system_get_size_of_block(beginning_of_block))
           - get_occupied_block_service_block_size();
}
#pragma endregion

size_t memory_with_buddy_system::get_number_in_bin_pow(char power) const {
    return 1 << power;
}

char memory_with_buddy_system::get_bin_pow_of_number(size_t number) const {
    return ceil(log2(number));
}

memory_with_buddy_system::memory_with_buddy_system(
        char pow,
        logger *logger = nullptr,
        memory *parent_allocator = nullptr)
{
    // can't even make a one available block (service block is needed)
    char available_block_service_block_size_power = get_bin_pow_of_number(get_available_block_service_block_size());
    if (pow < available_block_service_block_size_power) {
        this->debug_with_guard("Power of allocator will be set to " + std::to_string(available_block_service_block_size_power + 1)
                               + " for correct work of allocator");
        pow = available_block_service_block_size_power + 1;
    }

    // 2^pow >= available block service block
    size_t size_with_service_block = get_number_in_bin_pow(pow) + get_allocator_service_block_size();

    if (parent_allocator) {
        _ptr_to_allocator_metadata = parent_allocator->allocate(size_with_service_block);
    } else {
        try {
            _ptr_to_allocator_metadata = ::operator new(size_with_service_block);
        }
        catch (std::bad_alloc const &) {
            throw memory::Memory_exception("An object of buddy system allocator cannot be constructed");
        }
    }

    auto * allocator_pool_pow = reinterpret_cast<char *>(_ptr_to_allocator_metadata);
    *allocator_pool_pow = pow;

    auto * allocator_logger = reinterpret_cast<class logger **>(allocator_pool_pow + 1);
    *allocator_logger = logger;

    auto * allocator_parent_allocator = reinterpret_cast<memory **>(allocator_logger + 1);
    *allocator_parent_allocator = parent_allocator;

    auto * ptr_to_memory_pool = reinterpret_cast<void **>(allocator_parent_allocator + 1);
    *ptr_to_memory_pool = reinterpret_cast<void *>(reinterpret_cast<char *>(ptr_to_memory_pool) + sizeof(void *));

    void* first_block = *ptr_to_memory_pool;
    bool * is_available_first_block = reinterpret_cast<bool *>(first_block);
    *is_available_first_block = true;

    auto * power_first_block = reinterpret_cast<char *>(is_available_first_block + 1);
    *power_first_block = pow;

    void ** next_to_first_block = reinterpret_cast<void **>(power_first_block + 1);
    *next_to_first_block = nullptr;

    this->trace_with_guard("memory_with_buddy_system allocator was constructed");
}

memory_with_buddy_system::~memory_with_buddy_system()
{
    this->trace_with_guard("memory_with_sorted_list_deallocation allocator was destructed");

    auto * allocator_pool_pow = reinterpret_cast<char *>(_ptr_to_allocator_metadata);

    auto * allocator_logger = reinterpret_cast<logger **>(allocator_pool_pow + 1);

    auto * parent_allocator = *reinterpret_cast<memory **>(allocator_logger + 1);

    if (parent_allocator) {
        parent_allocator->deallocate(_ptr_to_allocator_metadata);
    } else {
        ::operator delete(_ptr_to_allocator_metadata);
    }
}

void *memory_with_buddy_system::allocate(size_t target_size) const {
    this->trace_with_guard("memory_with_buddy_system::allocate method execution started");

    if (!target_size) {
        this->warning_with_guard("Size of allocated block cannot be 0")
            ->trace_with_guard("memory_with_buddy_system::allocate method execution finished");
        return nullptr;
    }

    char power_needed = get_bin_pow_of_number(target_size + get_occupied_block_service_block_size()),
                   pool_power = *_buddy_system_get_ptr_size_of_allocator_pool(),
                   current_block_power = 0, target_block_power = 0;

    if (power_needed > pool_power) {
        this->warning_with_guard("No memory available to allocate, allocator pool is less than memory requested")
            ->trace_with_guard("memory_with_buddy_system::allocate method execution finished");

        throw memory::Memory_exception("A block in buddy system allocator cannot be allocated");
    }

    void * current_block = get_first_available_block_address(), * previous_to_current_block = nullptr,
         * previous_to_target_block = nullptr, * target_block = nullptr;

    while (current_block != nullptr) {
        current_block_power = *_buddy_system_get_size_of_block(current_block);
        if (current_block_power >= power_needed && (target_block_power == 0 || target_block_power > current_block_power))
        {
            target_block = current_block;
            target_block_power = current_block_power;
            previous_to_target_block = previous_to_current_block;
        }

        previous_to_current_block = current_block;
        current_block = get_next_available_block_address(current_block);
    }

    if (!target_block) {
        this->warning_with_guard("There is no memory available to allocate")
            ->trace_with_guard("memory_with_buddy_system::allocate method execution finished");

        throw memory::Memory_exception("A block in buddy system allocator cannot be allocated");
    }

    // delete target_block from list of available blocks
    // 0x2c7007339a9 target block
    void * next_to_target_block = get_next_available_block_address(target_block);
    if (previous_to_target_block) {
        *_buddy_system_get_available_block_address_field(previous_to_target_block) = next_to_target_block;
    }

    if (target_block == *_buddy_system_get_ptr_to_ptr_to_pool_start()) {
        *_buddy_system_get_ptr_to_ptr_to_pool_start() = next_to_target_block;
    }

    *_buddy_system_get_available_block_address_field(target_block) = nullptr;
    *_buddy_system_is_block_available(target_block) = false;

    // divide a block until target_block_power != (read: is bigger) power_needed
    void * buddy_to_target_block = nullptr, * allocator_pool_start = get_ptr_to_allocator_trusted_pool();
    char minimum_power = 0, available_service_bin_power = get_bin_pow_of_number(get_available_block_service_block_size());
    power_needed > available_service_bin_power ? minimum_power = power_needed : minimum_power = available_service_bin_power;

    while (target_block_power > minimum_power) {
        target_block_power -= 1;
        *_buddy_system_get_size_of_block(target_block) = target_block_power;

        // return buddy to available blocks list
        // | prev | target block might be divided | next | -> | prev | target block | buddy to target block | next |
        buddy_to_target_block = get_ptr_to_buddy(target_block, allocator_pool_start);
        *_buddy_system_is_block_available(buddy_to_target_block) = true;
        *_buddy_system_get_size_of_block(buddy_to_target_block) = target_block_power;

        if (previous_to_target_block) {
            *_buddy_system_get_available_block_address_field(previous_to_target_block) = buddy_to_target_block;
        } else {
            *_buddy_system_get_ptr_to_ptr_to_pool_start() = buddy_to_target_block;
        }
        *_buddy_system_get_available_block_address_field(buddy_to_target_block) = next_to_target_block;
        next_to_target_block = buddy_to_target_block;
    }

    std::string target_block_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(target_block) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())));

    this->information_with_guard("Block of size = " + std::to_string(get_number_in_bin_pow(target_block_power))
                                 + " was allocated." + " Address: " + target_block_address)
        ->trace_with_guard("memory_with_buddy_system::allocate method execution started");

    void * to_return = reinterpret_cast<void *>(reinterpret_cast<char *>(reinterpret_cast<bool *>(target_block) + 1) + 1);
    return to_return;
}

void memory_with_buddy_system::deallocate(const void *const target_to_dealloc) const
{
    this->trace_with_guard("memory_with_buddy_system::deallocate method execution started");

    if (!target_to_dealloc) {
        this->warning_with_guard("Target to deallocate should not be nullptr")
            ->trace_with_guard("memory_with_buddy_system::deallocate method execution finished");
        return;
    }

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    // make target_to_dealloc to point to the beginning of the block
    auto * new_available_block = reinterpret_cast<void *>(reinterpret_cast<bool *>(
            reinterpret_cast<char *>(const_cast<void *>(target_to_dealloc)) - 1) - 1);

    std::string target_to_dealloc_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(new_available_block) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())));

    this->information_with_guard("memory block with address: " + target_to_dealloc_address + " was deallocated successfully");

    *_buddy_system_is_block_available(new_available_block) = true;

    void * to_delete = nullptr, * allocator_pool_start = get_ptr_to_allocator_trusted_pool();
    void * buddy_block = get_ptr_to_buddy(new_available_block, allocator_pool_start),
         * next_to_new_available_block = nullptr;

    char new_available_block_power = *_buddy_system_get_size_of_block(new_available_block),
         allocator_pool_power = *_buddy_system_get_ptr_size_of_allocator_pool();

    while (new_available_block_power != allocator_pool_power
           && *_buddy_system_is_block_available(buddy_block)
           && *_buddy_system_get_size_of_block(buddy_block) == new_available_block_power)
    {
        new_available_block_power++;
        next_to_new_available_block = get_next_available_block_address(buddy_block);
        // | new_available_block | buddy_block |
        if (new_available_block < buddy_block) {
            to_delete = buddy_block;
        } else {
            // | buddy_block | new_available_block |
            to_delete = new_available_block;
            new_available_block = buddy_block;
        }
        *_buddy_system_get_available_block_address_field(to_delete) = nullptr;
        // *_buddy_system_get_size_of_block(new_available_block) = new_available_block_power;
        *_buddy_system_get_size_of_block(new_available_block) = new_available_block_power;
        buddy_block = get_ptr_to_buddy(new_available_block, allocator_pool_start);
    }
    *_buddy_system_is_block_available(new_available_block) = true;
    *_buddy_system_get_size_of_block(new_available_block) = new_available_block_power;
    *_buddy_system_get_available_block_address_field(new_available_block) = next_to_new_available_block;

    // change link from previous available block to new available block
    void * previous_to_new_available_block = *_buddy_system_get_ptr_to_ptr_to_pool_start();
    void * next_to_previous_block = nullptr;

    // make new_available_block a first block in the list
    if (previous_to_new_available_block > new_available_block) {
        *_buddy_system_get_ptr_to_ptr_to_pool_start() = new_available_block;
    }
    // new_available_block is not a first block in the list
    else if (previous_to_new_available_block != new_available_block && new_available_block_power != allocator_pool_power) {
        while (previous_to_new_available_block) {
            next_to_previous_block = get_next_available_block_address(previous_to_new_available_block);
            if (previous_to_new_available_block < new_available_block && next_to_previous_block >= new_available_block)
            {
                *_buddy_system_get_available_block_address_field(previous_to_new_available_block) = new_available_block;
            }
            previous_to_new_available_block = next_to_previous_block;
        }
    }

    this->trace_with_guard("memory_with_buddy_system::deallocate method execution finished");
}

logger *memory_with_buddy_system::get_logger() const noexcept {
    return *get_ptr_logger_of_allocator();
}
