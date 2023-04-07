#include "memory_with_buddy_system.h"

int main()
{
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::trace)
            ->with_stream("2.txt", Logger::Severity::information)
            ->with_stream("3.txt", Logger::Severity::critical)
            ->build();

    Memory *buddy_system_allocator = new memory_with_boundary_tags
            (10000, Memory::Allocation_strategy::best_fit, log, nullptr);
    buddy_system_allocator->set_logger(log);

    int size = 10;
    int* array = reinterpret_cast<int *>(buddy_system_allocator->allocate(sizeof(int) * size));
    for (auto i = 0; i < size; i++) {
        array[i] = size + i;
    }

    buddy_system_allocator->deallocate(array);
    std::cout << "oops =)" << std::endl;


    delete builder;
    delete log;
    delete buddy_system_allocator;
    return 0;
}