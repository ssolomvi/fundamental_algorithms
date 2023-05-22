#include "memory_with_sorted_list_deallocation.h"

int main() {
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::information)
            ->with_stream("2.txt", Logger::Severity::trace)
            ->build();

    Memory *sorted_list_allocator = new memory_with_sorted_list_deallocation
            (500, Memory::Allocation_strategy::worst_fit, log, nullptr);
//    sorted_list_allocator->set_logger(log);

    int size = 10;
/*
    char ** ch_ch_array = reinterpret_cast<char **>(sorted_list_allocator->allocate(size * sizeof(char *)));
    for (auto i = 0; i < size; i++) {
        ch_ch_array[i] = reinterpret_cast<char *>(sorted_list_allocator->allocate(size * sizeof(char)));
    }
    for (auto i = 0; i < size; i++) {
        sorted_list_allocator->deallocate(ch_ch_array[i]);
    }
    sorted_list_allocator->deallocate(ch_ch_array);
*/
    int *first = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size));
    int *second = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size * 2));
    int *third = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size));

    // 1: insert 3, delete second.
    sorted_list_allocator->deallocate(second);
    int *new_block = nullptr;
    // a) precisely
//    new_block = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size * 2));
    // b) leftover > 8 (occupied service block size)
//    new_block = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size));
    // c) leftover < 8
//    new_block = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size * 2 - 4));
    // d) bigger than second
    new_block = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size * 3));
    sorted_list_allocator->deallocate(new_block);

//    sorted_list_allocator->deallocate(first);


    /*
    int* ch_ch_array = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size));
    for (auto i = 0; i < size; i++) {
        ch_ch_array[i] = size + i;
    }
    sorted_list_allocator->deallocate(ch_ch_array);

    char * ch_array = reinterpret_cast<char *>(sorted_list_allocator->allocate(sizeof(char) * size));
    for (auto i = 0; i < size; i++) {
        ch_array[i] = size + i;
    }
    sorted_list_allocator->deallocate(ch_array);
*/
//    sorted_list_allocator->deallocate(second);
//    sorted_list_allocator->deallocate(third);

    delete builder;
    delete sorted_list_allocator;
    delete log;
    std::cout << "oops =)" << std::endl;
    return 0;
}