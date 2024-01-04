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

    enum class MessageType
    {
        Board,
        Id,
        Result
    };

    class Game {
    private:
        Board board;
        int turn = 1;
        int playerOnTurn = 0;
        bool gameOver = false;
        bool idsSent = false;

        std::vector<Player*> players;

        std::vector<std::thread*> playerThreads;
        std::thread* boardUpdateThread;

        std::condition_variable cvUpdateBoard;
        std::condition_variable cvManagePlayerTurn;
        bool updBoard = true;
        bool manPlayerTurns = false;

    public:
        const std::string END_MESSAGE = "%%";

        Game(std::vector<int> clientSockets);
        ~Game();
        void Begin();
        void End();
        void UpdateBoard();
        void ManagePlayerTurn(Player* player);
    };

}
#endif //SERVER_GAME_H
