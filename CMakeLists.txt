cmake_minimum_required(VERSION 3.14)
project(cpp_playground)
set(CMAKE_CXX_STANDARD 11)
add_executable(agnostic_vector src/agnostic_vector/main.cpp
        src/agnostic_vector/ComponentManager.cpp
        src/agnostic_vector/ComponentManager.h)