#include "memory_from_global_heap.h"

int main()
{
    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::debug)
            ->with_stream("console", Logger::Severity::trace)
            ->with_stream("2.txt", Logger::Severity::information)
            ->with_stream("3.txt", Logger::Severity::critical)
            ->build();

    Memory *global_heap_allocator = new memory_from_global_heap();
    global_heap_allocator->set_logger(log);

    int size = 10;
    int* array = reinterpret_cast<int *>(global_heap_allocator->allocate(sizeof(int) * size));
    for (auto i = 0; i < size; i++) {
        array[i] = size + i;
    }

    //global_heap_allocator->dump_occupied_block_before_deallocate(array);
    global_heap_allocator->deallocate(array);
    std::cout << "oops =)" << std::endl;

    delete builder;
    delete log;
    delete global_heap_allocator;

    return 0;
}
