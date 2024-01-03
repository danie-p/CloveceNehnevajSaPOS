cmake_minimum_required(VERSION 3.18)
project(Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server ServerMain.cpp
        active_socket.c
        active_socket.h
        buffer.h
        char_buffer.c
        char_buffer.h
        linked_list.h
        passive_socket.c
        passive_socket.h)

target_link_libraries(Server pthread)
