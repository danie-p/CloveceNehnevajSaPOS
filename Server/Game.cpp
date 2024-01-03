//
// Created by Kristian on 3. 1. 2024.
//

#include "Game.h"

namespace Server {
    Game::Game(std::vector<int> clientSockets) {
        this->playerSockets = clientSockets;
    }

    void Game::Begin() {
        for (int i = 0; i < playerSockets.size(); ++i) {
            playerThreads.push_back(new std::thread());
        }
    }

    void Game::End() {
        for (int i = 0; i < playerSockets.size(); ++i) {
            playerThreads[i]->join();
            delete playerThreads[i];
        }
    }
}