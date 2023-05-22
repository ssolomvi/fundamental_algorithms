cmake_minimum_required(VERSION 3.22)
project(course_work)

set(CMAKE_CXX_STANDARD 20)

add_executable(course_work
        main.cpp
        db/data_base.cpp db/data_base.h
        db_user_communication.cpp db_user_communication.h
        db_value/db_value.cpp db_value/db_value.h
        db_value/db_value_builder.cpp db_value/db_value_builder.h
        logger/logger.h logger/logger.cpp
        logger/logger_builder.h logger/logger_builder.cpp
        logger/logger_holder.h logger/logger_holder.cpp
        allocator/memory_base_class.h allocator/memory_base_class.cpp
        allocator/memory_holder.h allocator/memory_holder.cpp
        allocator_from_global_heap/memory_from_global_heap.h allocator_from_global_heap/memory_from_global_heap.cpp
        allocator_with_sorted_list_deallocation/memory_with_sorted_list_deallocation.h allocator_with_sorted_list_deallocation/memory_with_sorted_list_deallocation.cpp
        allocator_with_boundary_tags_deallocation/memory_with_boundary_tags.h allocator_with_boundary_tags_deallocation/memory_with_boundary_tags.cpp
        allocator_with_buddy_system/memory_with_buddy_system.h allocator_with_buddy_system/memory_with_buddy_system.cpp
        binary_tree/bs_tree.h binary_tree/associative_container.h
        avl_tree/avl_tree.h
        splay_tree/splay_tree.h
        rb_tree/rb_tree.h)
