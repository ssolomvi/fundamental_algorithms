#include <list>
#include <random>
//#include <cmath>

#include "allocator/memory_base_class.h"
#include "allocator/memory_holder.h"
#include "logger/logger.h"
#include "logger/logger_builder.h"
#include "logger/logger_holder.h"
#include "binary_tree/associative_container.h"
#include "binary_tree/bs_tree.h"
#include "splay_tree/splay_tree.h"
#include "avl_tree/avl_tree.h"
#include "rb_tree/rb_tree.h"
#include "allocator_from_global_heap/memory_from_global_heap.h"
#include "allocator_with_sorted_list_deallocation/memory_with_sorted_list_deallocation.h"
#include "allocator_with_boundary_tags_deallocation/memory_with_boundary_tags.h"
#include "allocator_with_buddy_system/memory_with_buddy_system.h"

class int_comparer
{
public:
    int operator()(int x, int y)
    {
        return x - y;
    }
};


void bst_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *binary_logger_builder = new logger_builder_impl();
    logger *binary_tree_logger = binary_logger_builder
            ->with_stream("rbtree_tests.txt", logger::severity::trace)
            ->build();
    delete binary_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
//    associative_container<int, std::string> *tree = new red_black_tree<int, std::string, int_comparer>(allocator, rbtree_logger);
    associative_container<int, std::string> *tree = new bs_tree<int, std::string, int_comparer>(binary_tree_logger, allocator);

    srand((unsigned)time(nullptr));

    int i = 1;
    while (1)
    {
        std::cout << "----------------------------------------" << std::endl << "Iteration #" << i << std::endl;

        auto key = rand() % 500;
        switch (rand() % 2)
        {
            case 0:
                try
                {
                    tree->insert(key, "");
                    std::cout << "successfully inserted key \"" << key << "\"" << std::endl;
                }
                catch (bs_tree<int, std::string, int_comparer>::bst_exception const &)
                {
                    std::cout << "insertion error - duplicate key \"" << key << "\" detected" << std::endl;
                }
                catch (memory::memory_exception const &)
                {
                    std::cout << "insertion error - allocation failed" << std::endl;
                }
                break;
            case 1:
                try
                {
                    tree->remove(key);
                    std::cout << "successfully removed key \"" << key << "\"" << std::endl;
                }
                catch (bs_tree<int, std::string, int_comparer>::bst_exception const &)
                {
                    std::cout << "removing error - key \"" << key << "\" not found" << std::endl;
                }
                break;
        }

        if (i++ % 100 == 0)
        {
            std::cout << "Tree state after iteration #" << i << ":" << std::endl;

            auto *bst = reinterpret_cast<bs_tree<int, std::string, int_comparer> *>(tree);
            auto end_prefix = bst->end_prefix();
            for (auto it = bst->begin_prefix(); it != end_prefix; ++it)
            {
                for (auto x = 0; x < std::get<0>(*it); x++)
                {
                    std::cout << "    ";
                }

                std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
            }
        }
    }

    delete tree;
    delete allocator;
    delete binary_tree_logger;
    delete allocator_logger;
}

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
                catch (memory::memory_exception const &ex)
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

void my_bst_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *binary_logger_builder = new logger_builder_impl();
    logger *binary_tree_logger = binary_logger_builder
            ->with_stream("bst_tests.txt", logger::severity::trace)
            ->build();
    delete binary_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
    bs_tree<int, std::string, int_comparer> BST_tree_initial(binary_tree_logger, allocator);
    bs_tree<int, std::string, int_comparer> BST_tree = BST_tree_initial;
    std::string ptr_value;
    try {
        ptr_value = std::move(BST_tree.remove(4));
        std::cout << ptr_value << std::endl;

    }
    catch (bs_tree<int, std::string, int_comparer>::bst_exception const &ex) {
        std::cout << ex.what() << std::endl;
    }

    BST_tree.insert(4, "A");
    BST_tree.insert(2, "B");
    BST_tree.insert(3, "C");
    BST_tree.insert(1, "D");
    BST_tree.insert(5, "E");
    BST_tree.insert(7, "F");
    BST_tree.insert(6, "G");

    auto ptr_end_prefix = BST_tree.end_prefix();
    for (auto it = BST_tree.begin_prefix(); it != ptr_end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    ptr_value = BST_tree.remove(4);
    std::cout << ptr_value << std::endl;
    ptr_value = BST_tree.remove(3);
    std::cout << ptr_value << std::endl;
    ptr_value = BST_tree.remove(5);
    std::cout << ptr_value << std::endl;
    ptr_value = BST_tree.remove(2);
    std::cout << ptr_value << std::endl;

    for (auto it = BST_tree.begin_prefix(); it != ptr_end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

//    delete BST_tree;
    delete binary_tree_logger;
    delete allocator;
    delete allocator_logger;
}

void splay_tree_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_splay_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *splay_tree_logger_builder = new logger_builder_impl();
    logger *splay_tree_logger = splay_tree_logger_builder
            ->with_stream("splay_t_tests.txt", logger::severity::trace)
            ->build();
    delete splay_tree_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
//    bs_tree<int, std::string, int_comparer> * sp_tree = new splay_tree<int, std::string, int_comparer>(splay_tree_logger, allocator);
    splay_tree<int, std::string, int_comparer> sp_tree_initial(splay_tree_logger, allocator);
    splay_tree<int, std::string, int_comparer> sp_tree = sp_tree_initial;

#pragma region rule 5 splay tree test
    sp_tree.insert(4, "a");
    sp_tree.insert(2, "b");
    sp_tree.insert(3, "c");
    sp_tree.insert(1, "d");
    sp_tree.insert(5, "e");
    sp_tree.insert(7, "f");
    sp_tree.insert(6, "g");

    auto end_prefix = sp_tree.end_prefix();
    for (auto it = sp_tree.begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    auto value = sp_tree.remove(4);
    std::cout << value << std::endl;
    value = sp_tree.remove(3);
    std::cout << value << std::endl;
    value = sp_tree.remove(1);
    std::cout << value << std::endl;

    for (auto it = sp_tree.begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

//    delete sp_tree;
    delete splay_tree_logger;
    delete allocator;
    delete allocator_logger;
#pragma endregion

    /*
    sp_tree->insert(4, "a");
    sp_tree->insert(2, "b");
    sp_tree->insert(3, "c");
    sp_tree->insert(1, "d");
    sp_tree->insert(5, "e");
    sp_tree->insert(7, "f");
    sp_tree->insert(6, "g");

    auto end_prefix = sp_tree->end_prefix();
    for (auto it = sp_tree->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    auto value = sp_tree->remove(4);
    std::cout << value << std::endl;
    value = sp_tree->remove(3);
    std::cout << value << std::endl;
    value = sp_tree->remove(1);
    std::cout << value << std::endl;

    for (auto it = sp_tree->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    delete sp_tree;
    delete splay_tree_logger;
    delete allocator;
    delete allocator_logger;
     */
}

void avl_tree_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_avl_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *avl_tree_logger_builder = new logger_builder_impl();
    logger *avl_tree_logger = avl_tree_logger_builder
            ->with_stream("avl_t_tests.txt", logger::severity::trace)
            ->build();
    delete avl_tree_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
    bs_tree<int, std::string, int_comparer> * avl_t = new avl_tree<int, std::string, int_comparer>(avl_tree_logger, allocator);

    avl_t->insert(4, "a");
    avl_t->insert(2, "b");
    avl_t->insert(3, "c");
    avl_t->insert(1, "d");
    avl_t->insert(5, "e");
    avl_t->insert(7, "f");
    avl_t->insert(6, "g");

    auto end_prefix = avl_t->end_prefix();
    for (auto it = avl_t->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    auto value = avl_t->remove(4);
    std::cout << value << std::endl;
    value = avl_t->remove(3);
    std::cout << value << std::endl;
    value = avl_t->remove(1);
    std::cout << value << std::endl;

    for (auto it = avl_t->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    delete avl_t;
    delete avl_tree_logger;
    delete allocator;
    delete allocator_logger;
}

void rb_tree_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_rb_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *rb_tree_logger_builder = new logger_builder_impl();
    logger *rb_tree_logger = rb_tree_logger_builder
            ->with_stream("rb_t_tests.txt", logger::severity::trace)
            ->build();
    delete rb_tree_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
    bs_tree<int, std::string, int_comparer> * rb_t = new rb_tree<int, std::string, int_comparer>(rb_tree_logger, allocator);

    rb_t->insert(4, "a");
    rb_t->remove(4);
    rb_t->insert(2, "b");
    rb_t->insert(3, "c");
    rb_t->insert(1, "d");
    rb_t->insert(5, "e");
    rb_t->insert(7, "f");
    rb_t->insert(6, "g");

    auto end_prefix = rb_t->end_prefix();
    for (auto it = rb_t->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    auto value = rb_t->remove(4);
    std::cout << value << std::endl;
    value = rb_t->remove(3);
    std::cout << value << std::endl;
    value = rb_t->remove(1);
    std::cout << value << std::endl;

    for (auto it = rb_t->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    delete rb_t;
    delete rb_tree_logger;
    delete allocator;
    delete allocator_logger;
}


int main()
{
//    bst_test();
//    allocators_demo(250000, memory::worst_fit, 7500);
//    splay_tree_test();
    my_bst_test();
//    avl_tree_test();
//    rb_tree_test();

    return 0;
}
