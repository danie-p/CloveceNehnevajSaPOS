//
// Created by Kristian on 3. 1. 2024.
//

#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        this->playerSockets = clientSockets;
        playerCount = clientSockets.size();
    }

    Game::~Game() {
        for (int i = 0; i < playerCount; ++i) {
            if (playerThreads[i] != nullptr) {
                playerThreads[i]->join();
                delete playerThreads[i];
                playerThreads[i] = nullptr;
            }
        }

        if (boardUpdateThread != nullptr) {
            boardUpdateThread->join();
            delete boardUpdateThread;
            boardUpdateThread = nullptr;
        }
    }

    void Game::Begin() {
        for (int i = 0; i < playerCount; ++i) {
            playerThreads.push_back(new std::thread(&Game::ManagePlayerTurns, this, playerSockets[i]));
        }

        boardUpdateThread = new std::thread(&Game::UpdateBoard, this);
    }

    void Game::End() {
        for (int i = 0; i < playerCount; ++i) {
            playerThreads[i]->join();
            delete playerThreads[i];
            playerThreads[i] = nullptr;
        }

        boardUpdateThread->join();
        delete boardUpdateThread;
        boardUpdateThread = nullptr;
    }

    void Game::ManagePlayerTurns(int socket) {

    }

    void Game::UpdateBoard() {
        // after applying the move of player on the board = after turn, send the board to players
        while (true) {
            {
                // first wait until the turn is made
                std::unique_lock<std::mutex> lock(mutex);
                while (!turnDone)
                    cvUpdateBoard.wait(lock);

                for (int i = 0; i < playerCount; ++i) {
                    if (/* not game over */1 == 2) {
                        //std::string boardStr = board.toString();
                        //write(playerSockets[i], boardStr.c_str(), boardStr.size() + 1);
                    } else {
                        // send results and return
                        return;
                    }
                }

                turnDone = false;
                turn++;
            }
        }
    }
}