cmake_minimum_required(VERSION 2.8)

project(btree)
add_executable(gen_num gen_num.cpp)
add_executable(btree btree.cpp)
add_executable(disk_btree disk_btree.cpp)
add_executable(show_file show_file.cpp)

