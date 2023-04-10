cmake_minimum_required(VERSION 3.22)
project(lab5)

# find_package(RapidJSON CONFIG REQUIRED)
# target_link_libraries(main PRIVATE rapidjson)
# include_directories(vcpkg/installed/x86-windows/include/rapidjson)

set(CMAKE_CXX_STANDARD 20)

# task 1 executables are below
#add_executable(lab5 base_classes/Logger.h lab5task1/logger_showcase.cpp)

# task 2 executables are below
add_executable(lab5 base_classes/Logger.h
        base_classes/Memory_base_class.h base_classes/Memory_base_class.cpp
        lab5task2/memory_from_global_heap.h lab5task2/memory_from_global_heap.cpp
        lab5task2/memory_from_global_heap_showcase.cpp base_classes/Logger.cpp)

# task 3 executables are below
#add_executable(lab5 base_classes/Logger.h
#        base_classes/Memory_base_class.h base_classes/Memory_base_class.cpp
#        lab5task3/memory_with_sorted_list_deallocation.h lab5task3/memory_with_sorted_list_deallocation.cpp
#        lab5task3/memory_with_sorted_list_deallocation_showcase.cpp)

# task 4 executables are below
#add_executable(lab5 base_classes/Logger.h
#        base_classes/Memory_base_class.h base_classes/Memory_base_class.cpp
#        lab5task4/memory_with_boundary_tags.h lab5task4/memory_with_boundary_tags.cpp
#        lab5task4/memory_with_boundary_tags_showcase.cpp)

# task 5 executables are below
#add_executable(lab5 base_classes/Logger.h
#        base_classes/Memory_base_class.h base_classes/Memory_base_class.cpp
#        lab5task5/memory_with_buddy_system.h lab5task5/memory_with_buddy_system.cpp
#        lab5task5/memory_with_buddy_system_showcase.cpp)


