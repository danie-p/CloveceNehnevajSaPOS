//
// Created by Kristian on 3. 1. 2024.
//

#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        char colors[] = {'R', 'G', 'B', 'Y'};
        for (int i = 0; i < clientSockets.size(); ++i) {
            players.push_back(new Player(i + 1, clientSockets[i], 1, colors[i]));
        }
    }

    Game::~Game() {
        for (int i = 0; i < players.size(); ++i) {
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

        for (int i = 0; i < players.size(); ++i) {
            delete players[i];
            players[i] = nullptr;
        }
    }

    void Game::Begin() {
        for (int i = 0; i < players.size(); ++i) {
            playerThreads.push_back(new std::thread(&Game::ManagePlayerTurn, this, players[i]));
        }

        boardUpdateThread = new std::thread(&Game::UpdateBoard, this);
    }

    void Game::End() {
        for (int i = 0; i < players.size(); ++i) {
            playerThreads[i]->join();
            delete playerThreads[i];
            playerThreads[i] = nullptr;
        }

        boardUpdateThread->join();
        delete boardUpdateThread;
        boardUpdateThread = nullptr;
    }

    void Game::UpdateBoard() {
        // first send ids to clients
        if (!idsSent) {
            for (int i = 0; i < players.size(); ++i) {
                std::string message = "$id:";
                std::string id = std::to_string(players[i]->getId());
                message += id;
                message += END_MESSAGE;

                {
                    std::unique_lock<std::mutex> lock(*players[i]->getMutex());
                    while (!updBoard)
                        cvUpdateBoard.wait(lock);

                    write(players[i]->getSocket(), message.c_str(), message.size() + 1);
                }

            }
            idsSent = true;
        }

        while (!gameOver) {
            for (int i = 0; i < players.size(); ++i) {
                std::string message = "$board:";
                std::string boardStr = board.toString();
                message += boardStr;
                message += END_MESSAGE;

                {
                    std::unique_lock<std::mutex> lock(*players[i]->getMutex());
                    while (!updBoard)
                        cvUpdateBoard.wait(lock);

                    write(players[i]->getSocket(), message.c_str(), message.size() + 1);

                    updBoard = false;
                    manPlayerTurns = true;
                    cvManagePlayerTurn.notify_one();
                }

            }
        }
    }

    void Game::ManagePlayerTurn(Player* player) {
        while (!gameOver)
        {
            std::string message = "$turn:";
            std::string boardStr = board.toString();
            message += boardStr;
            message += END_MESSAGE;

            {
                std::unique_lock<std::mutex> lock(*player->getMutex());
                while (!manPlayerTurns)
                    cvManagePlayerTurn.wait(lock);

                // now the player does a turn somehow
            }

        }
    }
}