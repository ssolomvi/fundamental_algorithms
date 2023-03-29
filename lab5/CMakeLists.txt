cmake_minimum_required(VERSION 3.22)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab5 lab5task2/memory_from_global_heap.h lab5task2/memory_from_global_heap_showcase.cpp
        base_classes/Memory_base_class.cpp base_classes/Memory_base_class.h base_classes/Logger.h
        lab5task3/memory_with_sorted_list_deallocation.cpp lab5task3/memory_with_sorted_list_deallocation.h lab5task4/memory_with_boundary_tags.cpp lab5task4/memory_with_boundary_tags.h)
