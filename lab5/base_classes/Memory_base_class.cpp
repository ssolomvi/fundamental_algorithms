#include "Memory_base_class.h"

void Memory::set_logger(Logger * const logger)
{
    _logger = logger;
}

Memory const * Memory::log_with_guard(std::string const & target, Logger::Severity severity) const
{
    if (_logger != nullptr) {
        _logger->log(target, severity);
    }
    return this;
}

void *const operator+=(const Memory &allocator, size_t target_size) {
    return allocator.allocate(target_size);
}

void operator-=(const Memory &allocator, const void *const target_to_dealloc) {
    allocator.deallocate(target_to_dealloc);
}

std::string Memory::address_to_hex(const void * ptr) const {
    char address_buf[(sizeof(void const * const) << 3) + 3];
    sprintf(address_buf, "0x%p", ptr);
    return std::string { address_buf };
}

void Memory::dump_occupied_block_before_deallocate_initial(void *const current_block_address,
                                                           const std::function<size_t(void * const)>& get_occupied_block_size) const {
    if (_logger == nullptr) {
        return;
    }

    auto const current_block_size = get_occupied_block_size(current_block_address);
    auto const * dump_iterator = reinterpret_cast<unsigned char *>(current_block_address);
    std::string result;

    for (auto i = 0; i < current_block_size; i++) {
        result += std::to_string(static_cast<unsigned short>(*dump_iterator++));

        if (i != current_block_size - 1) {
            result += ' ';
        }
    }

    this->log_with_guard("Memory at " + address_to_hex(current_block_address) + " = [" + result + "]",
                         Logger::Severity::trace);
}

Memory::Allocation_strategy Memory::get_allocation_mode() const {
    return this->allocationStrategy;
}


