#include "memory_with_boundary_tags.h"

int main()
{
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::information)
            ->with_stream("2.txt", Logger::Severity::trace)
            ->build();

    Memory *boundary_tags_allocator = new memory_with_boundary_tags
            (1000, Memory::Allocation_strategy::first_fit, log, nullptr);

    int size = 10;

    int *first = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size));
    int *second = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size * 2));

    int *third = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * (size - 2)));

    // 1: insert 3, delete second.
    boundary_tags_allocator->deallocate(second);
    int *new_block = nullptr;
    // a) precisely
//    new_block = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size * 2));
    // b) leftover > 8 (occupied service block size)
//    new_block = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size));
    // c) leftover < 8
//    new_block = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size * 2 - 4));
    // d) bigger than second
    new_block = reinterpret_cast<int *>(boundary_tags_allocator->allocate(sizeof(int) * size * 3));
    boundary_tags_allocator->deallocate(new_block);
    boundary_tags_allocator->deallocate(first);
/*
    char ** ch_ch_array = reinterpret_cast<char **>(boundary_tags_allocator->allocate(size * sizeof(char *)));
    for (auto i = 0; i < size; i++) {
        ch_ch_array[i] = reinterpret_cast<char *>(boundary_tags_allocator->allocate(size * sizeof(char)));
    }
    for (auto i = 0; i < size; i++) {
        boundary_tags_allocator->deallocate(ch_ch_array[i]);
    }
    boundary_tags_allocator->deallocate(ch_ch_array);
*/
    char * ch_array = reinterpret_cast<char *>(boundary_tags_allocator->allocate(sizeof(char) * size));
    for (auto i = 0; i < size; i++) {
        ch_array[i] = size + i;
    }
    boundary_tags_allocator->deallocate(ch_array);

    std::cout << "oops =)" << std::endl;

    delete builder;
    delete log;
    delete boundary_tags_allocator;
    return 0;
}