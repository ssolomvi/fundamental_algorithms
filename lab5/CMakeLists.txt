cmake_minimum_required(VERSION 3.22)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab5task2/memory_from_global_heap.h lab5task2/memory_from_global_heap_showcase.cpp
        base_classes/Memory_base_class.cpp base_classes/Memory_base_class.h base_classes/Logger.h)
