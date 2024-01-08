cmake_minimum_required(VERSION 3.18)
project(Client)

set(CMAKE_CXX_STANDARD 17)

add_executable(Client ClientMain.cpp
        ClientGame.cpp
        ClientGame.h
        MySocket.cpp
        MySocket.h
)

target_link_libraries(Client wsock32 ws2_32)


