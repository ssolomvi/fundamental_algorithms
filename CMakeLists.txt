cmake_minimum_required(VERSION 3.22)
project(fund_algths)

set(CMAKE_CXX_STANDARD 14)

include_directories(lab5/lab5task1)

add_executable(fund_algths
        lab5/lab5task1/main.cpp)
