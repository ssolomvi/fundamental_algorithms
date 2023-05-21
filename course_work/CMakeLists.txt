cmake_minimum_required(VERSION 3.22)
project(course_work)

set(CMAKE_CXX_STANDARD 20)

add_executable(course_work main.cpp db/data_base.cpp db/data_base.h db_user_communication.cpp db_user_communication.h)
