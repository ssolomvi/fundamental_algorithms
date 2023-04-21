#include <list>
#include <random>
#include <cmath>
#include "memory_from_global_heap.h"
#include "../lab5task3/memory_with_sorted_list_deallocation.h"
#include "../lab5task4/memory_with_boundary_tags.h"
#include "../lab5task5/memory_with_buddy_system.h"

//void bst_test()
//{
//    logger_builder *allocator_logger_builder = new logger_builder_impl();
//    logger *allocator_logger = allocator_logger_builder
//            ->with_stream("allocator_tests.txt", logger::severity::trace)
//            ->build();
//    delete allocator_logger_builder;
//
//    logger_builder *rbtree_logger_builder = new logger_builder_impl();
//    logger *rbtree_logger = rbtree_logger_builder
//            ->with_stream("rbtree_tests.txt", logger::severity::trace)
//            ->build();
//    delete rbtree_logger_builder;
//
//    memory *allocator = new memory_from_global_heap();
//    associative_container<int, std::string> *tree = new red_black_tree<int, std::string, int_comparer>(allocator, rbtree_logger);
//    srand((unsigned)time(nullptr));
//
//    int i = 1;
//    while (1)
//    {
//        std::cout << "----------------------------------------" << std::endl << "Iteration #" << i << std::endl;
//
//        auto key = rand() % 500;
//        switch (rand() % 2)
//        {
//            case 0:
//                try
//                {
//                    tree->insert(key, "");
//                    std::cout << "successfully inserted key \"" << key << "\"" << std::endl;
//                }
//                catch (binary_search_tree<int, std::string, int_comparer>::insertion_exception const &)
//                {
//                    std::cout << "insertion error - duplicate key \"" << key << "\" detected" << std::endl;
//                }
//                catch (memory::memory_exception const &)
//                {
//                    std::cout << "insertion error - allocation failed" << std::endl;
//                }
//                break;
//            case 1:
//                try
//                {
//                    tree->remove(key);
//                    std::cout << "successfully removed key \"" << key << "\"" << std::endl;
//                }
//                catch (binary_search_tree<int, std::string, int_comparer>::removing_exception const &)
//                {
//                    std::cout << "removing error - key \"" << key << "\" not found" << std::endl;
//                }
//                break;
//        }
//
//        if (i++ % 100 == 0)
//        {
//            std::cout << "Tree state after iteration #" << i << ":" << std::endl;
//
//            auto *bst = reinterpret_cast<binary_search_tree<int, std::string, int_comparer> *>(tree);
//            auto end_prefix = bst->end_prefix();
//            for (auto it = bst->begin_prefix(); it != end_prefix; ++it)
//            {
//                for (auto x = 0; x < std::get<0>(*it); x++)
//                {
//                    std::cout << "    ";
//                }
//
//                std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
//            }
//        }
//    }
//
//    delete tree;
//    delete allocator;
//    delete rbtree_logger;
//    delete allocator_logger;
//}

void allocator_demo(
        memory *allocator,
        unsigned int iterations_count)
{
    std::list<void *> allocated_blocks;
    std::random_device rd;
    auto mtgen = std::mt19937(rd());
    auto ud = std::uniform_int_distribution<>{0, 2};

    for (auto i = 0; i < iterations_count; i++)
    {
        switch (ud(mtgen))
        {
            case 0:
            case 1:
                try
                {
                    allocated_blocks.push_front(allocator->allocate(rand() % 251 + 50));
                    std::cout << "allocation succeeded" << std::endl;
                }
                catch (memory::Memory_exception const &ex)
                {
                    std::cout << ex.what() << std::endl;
                }
                catch (std::bad_alloc const &ex)
                {
                    std::cout << ex.what() << std::endl;
                }
                break;
            case 2:
                if (allocated_blocks.empty())
                {
                    std::cout << "No blocks to deallocate" << std::endl;

                    break;
                }

                auto it = allocated_blocks.begin();
                std::advance(it, rand() % allocated_blocks.size());
                allocator->deallocate(*it);
                allocated_blocks.erase(it);
                std::cout << "deallocation succeeded" << std::endl;
                break;
        }
    }

    while (!allocated_blocks.empty())
    {
        auto it = allocated_blocks.begin();
        std::advance(it, rand() % allocated_blocks.size());
        allocator->deallocate(*it);
        allocated_blocks.erase(it);
        std::cout << "deallocation succeeded" << std::endl;
    }
}

void allocators_demo(
        size_t trusted_memory_size,
        memory::Allocation_strategy fit_mode,
        unsigned int iterations_count)
{
    logger_builder *builder = new logger_builder_impl();
    auto *global_heap_allocator_logger = builder
            ->with_stream("global heap allocator.txt", logger::severity::trace)
            ->build();
    delete builder;
//    builder->clear();

    memory *global_heap_allocator = new memory_from_global_heap(global_heap_allocator_logger);

    builder = new logger_builder_impl();
    auto *sorted_list_allocator_logger = builder
            ->with_stream("sorted list allocator.txt", logger::severity::trace)
            ->build();
//    builder->clear();
    delete builder;

    memory *sorted_list_allocator = new memory_with_sorted_list_deallocation(trusted_memory_size, fit_mode, sorted_list_allocator_logger, nullptr);

    builder = new logger_builder_impl();
    auto *boundary_tags_allocator_logger = builder
            ->with_stream("boundary tags allocator.txt", logger::severity::trace)
            ->build();
//    builder->clear();
    delete builder;
    memory *boundary_tags_allocator = new memory_with_boundary_tags(trusted_memory_size, fit_mode, boundary_tags_allocator_logger, nullptr );

    builder = new logger_builder_impl();
    auto *buddy_system_allocator_logger = builder
            ->with_stream("buddy system allocator.txt", logger::severity::trace)
            ->build();
    delete builder;
    memory *buddy_system_allocator = new memory_with_buddy_system(static_cast<size_t>(std::log2(trusted_memory_size)), buddy_system_allocator_logger, nullptr);

    allocator_demo(global_heap_allocator, iterations_count);
    delete global_heap_allocator;
    delete global_heap_allocator_logger;

    allocator_demo(sorted_list_allocator, iterations_count);
    delete sorted_list_allocator;
    delete sorted_list_allocator_logger;

    allocator_demo(boundary_tags_allocator, iterations_count);
    delete boundary_tags_allocator;
    delete boundary_tags_allocator_logger;

    allocator_demo(buddy_system_allocator, iterations_count);
    delete buddy_system_allocator;
    delete buddy_system_allocator_logger;
}

int main()
{
    allocators_demo(250000, memory::worst_fit, 7500);

    return 0;
}
