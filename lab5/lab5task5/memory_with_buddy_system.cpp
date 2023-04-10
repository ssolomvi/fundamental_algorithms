#include "memory_with_buddy_system.h"

#pragma region Allocator properties
size_t memory_with_buddy_system::get_allocator_service_block_size() const {
    return sizeof(unsigned short) + sizeof(Logger *) + sizeof(Memory *) + sizeof(void *);
}

unsigned short *memory_with_buddy_system::_buddy_system_get_ptr_size_of_allocator_pool() const {
    return reinterpret_cast<unsigned short *>(_ptr_to_allocator_metadata);
}

Logger **memory_with_buddy_system::_buddy_system_get_ptr_logger_of_allocator() const {
    return reinterpret_cast<Logger **>(_buddy_system_get_ptr_size_of_allocator_pool() + 1);
}

Memory **memory_with_buddy_system::_buddy_system_get_ptr_to_ptr_parent_allocator() const {
    return reinterpret_cast<Memory **>(_buddy_system_get_ptr_logger_of_allocator() + 1);
}

void **memory_with_buddy_system::_buddy_system_get_ptr_to_ptr_to_pool_start() const {
    return reinterpret_cast<void **>(_buddy_system_get_ptr_to_ptr_parent_allocator() + 1);
}
#pragma endregion

#pragma region Buddy system block properties
bool * memory_with_buddy_system::_buddy_system_is_block_available(void *block) const {
    return reinterpret_cast<bool *>(block);
}

unsigned short * memory_with_buddy_system::_buddy_system_get_size_of_block(void *block) const {
    return reinterpret_cast<unsigned short *>(_buddy_system_is_block_available(block) + 1);
}

void **memory_with_buddy_system::_buddy_system_get_available_block_address_field(void * block) const {
    return reinterpret_cast<void **>(_buddy_system_get_size_of_block(block));
}

void **memory_with_buddy_system::get_ptr_to_buddy(void *block) const {
    return reinterpret_cast<void **>(
            reinterpret_cast<size_t>(block) ^ get_number_in_bin_pow(*_buddy_system_get_size_of_block(block)));
}
#pragma endregion

#pragma region Available block methods
void *memory_with_buddy_system::get_first_available_block_address() const {
    return *_buddy_system_get_ptr_to_ptr_to_pool_start();
}

size_t memory_with_buddy_system::get_available_block_service_block_size() const {
    return sizeof(bool) + sizeof(unsigned short) + sizeof(void *);
}

void *memory_with_buddy_system::get_next_available_block_address(void *memory_block) const {
    return *reinterpret_cast<void **>(
            reinterpret_cast<unsigned short *>(
                    reinterpret_cast<bool *>(memory_block) + 1)
            + 1);
}
#pragma endregion

#pragma region Occupied block methods
size_t memory_with_buddy_system::get_occupied_block_service_block_size() const {
    return sizeof(bool) + sizeof(unsigned short);
}

size_t memory_with_buddy_system::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return get_number_in_bin_pow(*_buddy_system_get_size_of_block(occupied_block))
           - get_occupied_block_service_block_size();
}
#pragma endregion

size_t memory_with_buddy_system::get_number_in_bin_pow(size_t power) const
{
    return 1 << power;
}

unsigned short memory_with_buddy_system::get_bin_pow_of_number(size_t number) const {
    return ceil(log2(number));
}

memory_with_buddy_system::memory_with_buddy_system(
        unsigned short pow,
        Logger *logger = nullptr,
        Memory *parent_allocator = nullptr)
{
    // can't even make a one available block (service block is needed)
    if (pow < get_bin_pow_of_number(get_available_block_service_block_size())) {
        // TODO: throw a message, can't make allocator
    }

    // 2^pow >= available block service block
    size_t size_with_service_block = get_number_in_bin_pow(pow) + get_allocator_service_block_size();

    if (parent_allocator) {
        _ptr_to_allocator_metadata = parent_allocator->allocate(size_with_service_block);
    } else {
        _ptr_to_allocator_metadata = ::operator new(size_with_service_block);
    }

    *(_buddy_system_get_ptr_size_of_allocator_pool()) = pow;
    *(_buddy_system_get_ptr_logger_of_allocator()) = logger;
    *(_buddy_system_get_ptr_to_ptr_parent_allocator()) = parent_allocator;
    *(_buddy_system_get_ptr_to_ptr_to_pool_start()) = (_buddy_system_get_ptr_to_ptr_to_pool_start() + 1);

    void * first_block = *(_buddy_system_get_ptr_to_ptr_to_pool_start());
    bool * is_available_first_block = reinterpret_cast<bool *>(first_block);
    *is_available_first_block = true;
    unsigned short * power_first_block = reinterpret_cast<unsigned short *>(is_available_first_block + 1);
    *power_first_block = pow;
    void ** next_to_first_block = reinterpret_cast<void **>(power_first_block + 1);
    *next_to_first_block = nullptr;

    this->log_with_guard("memory_with_buddy_system allocator was constructed",
                         Logger::Severity::trace);
}

memory_with_buddy_system::~memory_with_buddy_system()
{
    this->log_with_guard("memory_with_sorted_list_deallocation allocator was destructed",
                         Logger::Severity::trace);

    Memory *parent_allocator = *get_ptr_to_ptr_parent_allocator();
    if (parent_allocator) {
        parent_allocator->deallocate(this);
    }
}

void *memory_with_buddy_system::allocate(size_t target_size) const {
    this->log_with_guard("memory_with_buddy_system::allocate method execution started",
                         Logger::Severity::trace);

    if (!target_size) {
        this->log_with_guard("Size of allocated block cannot be 0", Logger::Severity::warning)
            ->log_with_guard("memory_with_buddy_system::allocate method execution finished", Logger::Severity::trace);
        // TODO: throw a message (?), target_size != 0
        return nullptr;
    }

    unsigned short power_needed = get_bin_pow_of_number(target_size + get_occupied_block_service_block_size()),
                   pool_power = *get_ptr_size_of_allocator_pool(),
                   current_block_power = 0, target_block_power = 0;

    if (power_needed > pool_power) {
        this->log_with_guard("No memory available to allocate, allocator pool is less than memory requested", Logger::Severity::warning)
            ->log_with_guard("memory_with_buddy_system::allocate method execution finished", Logger::Severity::trace);
        throw std::bad_alloc();
    }

    void * current_block = get_first_available_block_address(), * previous_to_current_block = nullptr,
         * previous_to_target_block = nullptr, * target_block = nullptr;

    while (current_block != nullptr) {
        current_block_power = *_buddy_system_get_size_of_block(current_block);
        if (current_block_power >= power_needed && (target_block_power == 0 || target_block_power > current_block_power)) {
            target_block = current_block;
            target_block_power = current_block_power;
            previous_to_target_block = previous_to_current_block;
        }

        previous_to_current_block = current_block;
        current_block = get_next_available_block_address(current_block);
    }

    if (!target_block) {
        this->log_with_guard("There is no memory available to allocate", Logger::Severity::warning)
            ->log_with_guard("memory_with_buddy_system::allocate method execution finished", Logger::Severity::trace);
        throw std::bad_alloc();
    }

    // delete target_block from list of available blocks
    void * next_to_target_block = get_next_available_block_address(target_block);
    *_buddy_system_get_available_block_address_field(previous_to_target_block) = next_to_target_block;
    *_buddy_system_get_available_block_address_field(target_block) = nullptr;
    *_buddy_system_is_block_available(target_block) = false;

    // divide a block until target_block_power != (read: is bigger) power_needed
    void * buddy_to_target_block = nullptr;
    unsigned short minimum_power = 0, available_service_bin_power = get_bin_pow_of_number(get_available_block_service_block_size());
    power_needed > available_service_bin_power ? minimum_power = power_needed : minimum_power = available_service_bin_power;

    while (target_block_power > minimum_power) {
        target_block_power -= 1;
        *_buddy_system_get_size_of_block(target_block) = target_block_power;

        // return buddy to available blocks list
        // | prev | target block might be divided | next | -> | prev | target block | buddy to target block | next |
        buddy_to_target_block = get_ptr_to_buddy(target_block);
        *_buddy_system_is_block_available(buddy_to_target_block) = true;
        *_buddy_system_get_size_of_block(buddy_to_target_block) = target_block_power;
        *_buddy_system_get_available_block_address_field(previous_to_target_block) = buddy_to_target_block;
        *_buddy_system_get_available_block_address_field(buddy_to_target_block) = next_to_target_block;
        next_to_target_block = buddy_to_target_block;
    }

    std::string target_block_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(target_block) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())));

    this->log_with_guard("Memory block with _size = " + std::to_string(get_number_in_bin_pow(target_block_power)) + " was allocated successfully", Logger::Severity::information)
        ->log_with_guard("Allocated block address: " + target_block_address, Logger::Severity::debug)
        ->log_with_guard("memory_with_buddy_system::allocate method execution started",
                         Logger::Severity::trace);

    return *reinterpret_cast<void **>(reinterpret_cast<unsigned short *>(reinterpret_cast<bool *>(target_block) + 1) + 1);
}

void memory_with_buddy_system::deallocate(const void *const target_to_dealloc) const
{
    this->log_with_guard("memory_with_buddy_system::deallocate method execution started",
                         Logger::Severity::trace);

    if (!target_to_dealloc) {
        this->log_with_guard("Target to deallocate should not be nullptr", Logger::Severity::warning)
                ->log_with_guard("memory_with_buddy_system::allocate method execution finished", Logger::Severity::trace);
        // TODO: message (?) target_to_dealloc is nullptr
        return;
    }

    // make target_to_dealloc to point to the beginning of the block
    *const_cast<void **>(&target_to_dealloc) = *reinterpret_cast<void **>(
            reinterpret_cast<unsigned short *>(
                    reinterpret_cast<bool *>(const_cast<void *>(target_to_dealloc)) - 1
                    ) - 1 );

    // TODO: is dump right?
    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));

    std::string target_to_dealloc_address = address_to_hex(reinterpret_cast<void *>(
            reinterpret_cast<char *>(const_cast<void *>(target_to_dealloc)) - reinterpret_cast<char *>(get_ptr_to_allocator_trusted_pool())));

    this->log_with_guard("Memory block with address: " + target_to_dealloc_address + " was deallocated successfully", Logger::Severity::information);

    *_buddy_system_is_block_available(const_cast<void *>(target_to_dealloc)) = true;

    void * new_available_block = const_cast<void *>(target_to_dealloc), * to_delete = nullptr;
    void * buddy_block = *get_ptr_to_buddy(new_available_block), * next_to_new_available_block = nullptr;

    unsigned short new_available_block_power = *_buddy_system_get_size_of_block(new_available_block),
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

        buddy_block = *get_ptr_to_buddy(new_available_block);
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

    this->log_with_guard("memory_with_buddy_system::deallocate method execution finished",
                         Logger::Severity::trace);
}
