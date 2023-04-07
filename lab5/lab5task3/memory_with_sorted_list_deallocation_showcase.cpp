#include "memory_with_sorted_list_deallocation.h"

int main()
{
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::trace)
            ->with_stream("2.txt", Logger::Severity::information)
            ->with_stream("3.txt", Logger::Severity::critical)
            ->build();

    Memory *sorted_list_allocator = new memory_with_sorted_list_deallocation
            (10000, Memory::Allocation_strategy::best_fit, log, nullptr);
    sorted_list_allocator->set_logger(log);

    int size = 10;
    int* array = reinterpret_cast<int *>(sorted_list_allocator->allocate(sizeof(int) * size));
    for (auto i = 0; i < size; i++) {
        array[i] = size + i;
    }

    sorted_list_allocator->deallocate(array);
    std::cout << "oops =)" << std::endl;


    delete builder;
    delete log;
    delete sorted_list_allocator;
    return 0;
}