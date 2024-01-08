cmake_minimum_required(VERSION 3.18)
project(Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server ServerMain.cpp
        Board.cpp
        Board.h
        Player.cpp
        Player.h
        Square.cpp
        Square.h
        ServerConnection.cpp
        ServerConnection.h
        Game.cpp
        Game.h)

target_link_libraries(Server pthread)
