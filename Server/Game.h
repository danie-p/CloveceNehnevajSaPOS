//
// Created by Kristian on 3. 1. 2024.
//

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include <thread>

#include "Board.h"

namespace Server {

    void ManagePlayerTurns(int socket);

    class Game {
    private:
        Board board;
        int playerOnTurn = 0;
        std::vector<int> playerSockets;
        std::vector<std::thread*> playerThreads;

    public:
        Game(std::vector<int> clientSockets);
        void Begin();
        void End();
    };

}
#endif //SERVER_GAME_H
