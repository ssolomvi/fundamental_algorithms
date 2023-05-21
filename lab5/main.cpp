#include <list>
#include <random>
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
#include "b_tree/b_tree.h"
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

typedef enum trees {
    BST,
    AVL,
    SPLAY,
    RB,
    B_TREE,
    B_PLUS
} trees;

void my_tree_test(unsigned iterations, memory* allocator, logger* tree_logger, trees tree_type, unsigned tree_parameter = 3)
{
    associative_container<int, char> *tree = nullptr;
    switch (tree_type) {
        case BST:
            tree = new bs_tree<int, char, int_comparer>(tree_logger, allocator);
            break;
        case AVL:
            tree = new avl_tree<int, char, int_comparer>(tree_logger, allocator);
            break;
        case SPLAY:
            tree = new splay_tree<int, char, int_comparer>(tree_logger, allocator);
            break;
        case RB:
            tree = new rb_tree<int, char, int_comparer>(tree_logger, allocator);
            break;
        case B_TREE:
            tree = new b_tree<int, char, int_comparer>(tree_parameter, tree_logger, allocator);
            break;
        default:
            return;
    }

    srand((unsigned)time(nullptr));

    int i = 1;
    while (1)
    {
        if (i == iterations)
            break;
        std::cout << "----------------------------------------" << std::endl << "Iteration #" << i << std::endl;

        auto key = rand() % 500;
        switch (rand() % 2)
        {
            case 0:
                try
                {
                    tree->insert(key, '\0');
                    std::cout << "successfully inserted key \"" << key << "\"" << std::endl;
                }
                catch (bs_tree<int, char, int_comparer>::insert_exception const &)
                {
                    std::cout << "insertion error - duplicate key \"" << key << "\" detected" << std::endl;
                }
                catch (b_tree<int, char, int_comparer>::insert_exception const &)
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
                catch (bs_tree<int, char, int_comparer>::remove_exception const &)
                {
                    std::cout << "removing error - key \"" << key << "\" not found" << std::endl;
                }
                catch (b_tree<int, char, int_comparer>::remove_exception const &) {
                    std::cout << "removing error - key \"" << key << "\" not found" << std::endl;
                }
                break;
        }

        if (i++ % 100 == 0)
        {
            std::cout << "Tree state after iteration #" << i << ":" << std::endl;

            if (tree_type == BST || tree_type == AVL || tree_type == SPLAY || tree_type == RB) {
                auto *bst = reinterpret_cast<bs_tree<int, char, int_comparer> *>(tree);
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
            else {
                auto *b_t = reinterpret_cast<b_tree<int, char, int_comparer> *>(tree);
                auto end_iteration = b_t->end_iter();
                unsigned count_of_keys_in_node, i = 0;
                for (auto it = b_t->begin_iter(); it != end_iteration; ++it)
                {
                    for (auto x = 0; x < std::get<0>(*it); x++)
                    {
                        std::cout << "    ";
                    }

                    count_of_keys_in_node = std::get<3>(*it);
                    for (i = 0; i < count_of_keys_in_node; i++) {
                        std::cout << "key: " << std::get<1>(*it)[i] << "; ";
//            std::cout << "key: " << std::get<1>(*it)[i] << ", value: \"" << std::get<2>(*it)[i] << "\" ";
                    }
                    std::cout << std::endl;
                }
            }


        }
    }

    delete tree;
}

void allocator_demo(memory *allocator, unsigned int iterations_count)
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

void allocators_demo(size_t trusted_memory_size, memory::Allocation_strategy fit_mode, unsigned int iterations_count)
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

void my_rb_tree_test()
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

    rb_t->insert(10, "a");
    rb_t->insert(3, "b");
    rb_t->insert(8, "c");
    rb_t->insert(7, "d");

    rb_t->remove(8);
    /*
    rb_t->insert(4, "a");
    rb_t->insert(2, "b");
    rb_t->insert(3, "c");
    rb_t->insert(1, "d");
    rb_t->insert(5, "e");
    rb_t->insert(7, "f");
    rb_t->insert(6, "g");
*/
    auto end_prefix = rb_t->end_prefix();
    for (auto it = rb_t->begin_prefix(); it != end_prefix; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        std::cout << "key: " << std::get<1>(*it) << ", value: \"" << std::get<2>(*it) << "\"" << std::endl;
    }

    auto value = rb_t->remove(2);
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

void print_b_tree(b_tree<int, char, int_comparer>* tree)
{
    auto end_iteration = tree->end_iter();
    unsigned count_of_keys_in_node, i = 0;
    for (auto it = tree->begin_iter(); it != end_iteration; ++it)
    {
        for (auto x = 0; x < std::get<0>(*it); x++)
        {
            std::cout << "    ";
        }

        count_of_keys_in_node = std::get<3>(*it);
        for (i = 0; i < count_of_keys_in_node; i++) {
            std::cout << "key: " << std::get<1>(*it)[i] << "; ";
//            std::cout << "key: " << std::get<1>(*it)[i] << ", value: \"" << std::get<2>(*it)[i] << "\" ";
        }
        std::cout << std::endl;
    }
}

void b_tree_tests(unsigned iterations, memory* allocator, logger* tree_logger, trees tree_type)
{

}

void my_b_tree_test()
{
    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_b_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *b_tree_logger_builder = new logger_builder_impl();
    logger *b_tree_logger = b_tree_logger_builder
            ->with_stream("b_t_tests.txt", logger::severity::trace)
            ->build();
    delete b_tree_logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);
    b_tree<int, char, int_comparer> * b_t = new b_tree<int, char, int_comparer>(2, b_tree_logger, allocator);

    b_t->insert(4, 'a');
    b_t->insert(2, 'b');
    b_t->insert(3, 'c');
    b_t->insert(1, 'd');
    b_t->insert(5, 'e');
    b_t->insert(7, 'f');
    b_t->insert(6, 'g');

    print_b_tree(b_t);

    auto value = b_t->remove(2);
    std::cout << value << std::endl;
    print_b_tree(b_t);
    value = b_t->remove(3);
    std::cout << value << std::endl;
    print_b_tree(b_t);
    value = b_t->remove(1);
    std::cout << value << std::endl;
    print_b_tree(b_t);

    delete b_t;
    delete b_tree_logger;
    delete allocator;
    delete allocator_logger;
}


int main()
{
#pragma region tree test
    unsigned iterations = 5001;

    logger_builder *allocator_logger_builder = new logger_builder_impl();
    logger *allocator_logger = allocator_logger_builder
            ->with_stream("allocator_tests.txt", logger::severity::trace)
            ->build();
    delete allocator_logger_builder;

    logger_builder *logger_builder = new logger_builder_impl();
    logger *tree_logger = logger_builder
            ->with_stream("tree_tests.txt", logger::severity::trace)
            ->build();
    delete logger_builder;

    memory *allocator = new memory_from_global_heap(allocator_logger);

    my_tree_test(iterations, allocator, tree_logger, B_TREE, 2);

    delete allocator;
    delete allocator_logger;
    delete tree_logger;
#pragma endregion

//    my_b_tree_test();
    return 0;
}
