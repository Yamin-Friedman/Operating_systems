cmake_minimum_required(VERSION 3.9)
project(wet2)

set(CMAKE_CXX_STANDARD 11)

add_executable(wet2 main.cpp ATM.h ATM.cpp Bank.cpp Bank.h Account.cpp Account.h)