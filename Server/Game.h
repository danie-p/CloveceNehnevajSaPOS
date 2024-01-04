//
// Created by Kristian on 3. 1. 2024.
//

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include "Board.h"

namespace Server {
    class Game {
    private:
        Board board;
        int turn = 1;
        int playerOnTurn = 0;
        int playerCount = 0;
        std::vector<int> playerSockets;
        std::vector<std::thread*> playerThreads;
        std::thread* boardUpdateThread;
        std::condition_variable cvUpdateBoard;
        std::mutex mutex;
        bool turnDone = true;
        bool gameOver = false;

    public:
        Game(std::vector<int> clientSockets);
        ~Game();
        void Begin();
        void End();
        void ManagePlayerTurns(int socket, int id);
        void UpdateBoard();
    };

}
#endif //SERVER_GAME_H
