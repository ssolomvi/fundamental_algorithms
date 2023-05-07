cmake_minimum_required(VERSION 3.22)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

# current configs
add_executable(lab5
        logger/logger.h logger/logger.cpp
        logger/logger_builder.h logger/logger_builder.cpp
        logger/logger_holder.h logger/logger_holder.cpp
        allocator/memory_base_class.h allocator/memory_base_class.cpp
        allocator/memory_holder.h allocator/memory_holder.cpp
        binary_tree/associative_container.h
        binary_tree/bs_tree.h
        splay_tree/splay_tree.h
        avl_tree/avl_tree.cpp avl_tree/avl_tree.h
        rb_tree/rb_tree.cpp rb_tree/rb_tree.h
        lab5task2/memory_from_global_heap.h lab5task2/memory_from_global_heap.cpp
        lab5task3/memory_with_sorted_list_deallocation.h lab5task3/memory_with_sorted_list_deallocation.cpp
        lab5task4/memory_with_boundary_tags.h lab5task4/memory_with_boundary_tags.cpp
        lab5task5/memory_with_buddy_system.h lab5task5/memory_with_buddy_system.cpp
        main.cpp )
