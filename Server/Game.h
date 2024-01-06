//
// Created by Kristian on 3. 1. 2024.
//

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <vector>
#include <thread>
#include <unistd.h>
#include <strings.h>
#include <iostream>
#include <condition_variable>

#include "Board.h"

namespace Server {

    class Game {
    private:
        int turn = 1;
        int playerIdOnTurn = 1;
        bool gameOver = false;

        std::vector<Player*> players;
        Board board{&players};

        std::thread* sendUpdateThread;
        std::thread* manageTurnThread;

        std::mutex dataLock;
        std::condition_variable cvSendUpdate;
        std::condition_variable cvManagePlayerTurn;
        bool updateSent = false;
        bool turnManaged = true;

    public:
        const std::string END_MESSAGE = "%";
        const std::string GAME_OVER = "GAME_OVER";

        Game(std::vector<int> clientSockets);
        ~Game();
        void Begin();
        void End();
        void SendUpdate();
        void ManagePlayerTurn();
    };

}
#endif //SERVER_GAME_H
