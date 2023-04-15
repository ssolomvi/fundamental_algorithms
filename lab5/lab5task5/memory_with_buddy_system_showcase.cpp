#include "memory_with_buddy_system.h"

int main()
{
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::information)
            ->with_stream("2.txt", Logger::Severity::trace)
            ->build();

    Memory *buddy_system_allocator = new memory_with_buddy_system
            (10, log, nullptr);

    int size = 3;

    char ** ch_ch_array = reinterpret_cast<char **>(buddy_system_allocator->allocate(size * sizeof(char *)));
    for (auto i = 0; i < size; i++) {
        ch_ch_array[i] = reinterpret_cast<char *>(buddy_system_allocator->allocate(size * sizeof(char)));
    }
    for (auto i = 0; i < size; i++) {
        buddy_system_allocator->deallocate(ch_ch_array[i]);
    }
    buddy_system_allocator->deallocate(ch_ch_array);


    int *first = reinterpret_cast<int *>(buddy_system_allocator->allocate(sizeof(int) * size));
    int *second = reinterpret_cast<int *>(buddy_system_allocator->allocate(sizeof(int) * size * 2));
    int *third = reinterpret_cast<int *>(buddy_system_allocator->allocate(sizeof(int) * size));

    buddy_system_allocator->deallocate(second);

    buddy_system_allocator->deallocate(first);

    buddy_system_allocator->deallocate(third);

    std::cout << "oops =)" << std::endl;

    delete builder;
    delete log;
    delete buddy_system_allocator;
    return 0;
}