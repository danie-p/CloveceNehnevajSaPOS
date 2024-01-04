//
// Created by Kristian on 3. 1. 2024.
//

#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        char colors[] = {'R', 'G', 'B', 'Y'};
        this->players = new std::vector<Player*>;
        for (int i = 0; i < clientSockets.size(); ++i) {
            players->push_back(new Player(i + 1, clientSockets[i], 1, colors[i]));
        }
        std::cout << "Game instance created\n";
    }

    Game::~Game() {
        for (int i = 0; i < players->size(); ++i) {
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

        for (auto & player : *players) {
            delete player;
            player = nullptr;
        }

        std::cout << "Game instance destroyed\n";
    }

    void Game::Begin() {
        std::cout << "Game starting..\n";

        for (auto & player : *players) {
            playerThreads.push_back(new std::thread(&Game::ManagePlayerTurn, this, player));
            std::cout << "Created turn management thread for player id " << player->getId() << "\n";
        }

        boardUpdateThread = new std::thread(&Game::SendUpdate, this);
        std::cout << "Create update thread\n";
    }

    void Game::End() {
        std::cout << "Game is ending...\n";

        for (int i = 0; i < players->size(); ++i) {
            playerThreads[i]->join();
            delete playerThreads[i];
            playerThreads[i] = nullptr;
        }

        boardUpdateThread->join();
        delete boardUpdateThread;
        boardUpdateThread = nullptr;
    }

    void Game::SendUpdate() {
        // first send ids to clients
        if (!idsSent) {
            std::cout << "Sending player ids to clients...\n";
            for (int i = 0; i < players->size(); ++i) {
                std::string message = "$id:";
                std::string id = std::to_string(players->at(i)->getId());
                message += id;
                message += END_MESSAGE;
                {
                    std::unique_lock<std::mutex> lock(*players->at(i)->getMutex());
                    while (!updBoard) {
                        cvUpdateBoard.wait(lock);
                    }
                    write(players->at(i)->getSocket(), message.c_str(), message.size() + 1);
                }
            }
            idsSent = true;
        }

        while (!gameOver) {
            for (int i = 0; i < players->size(); ++i) {
                std::string message = "$board:";
                std::string boardStr = board.toString();
                message += boardStr;
                message += END_MESSAGE;

                {
                    std::unique_lock<std::mutex> lock(*players->at(i)->getMutex());
                    while (!updBoard)
                        cvUpdateBoard.wait(lock);

                    write(players->at(i)->getSocket(), message.c_str(), message.size() + 1);

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
                while (!manPlayerTurns && playerIdOnTurn != player->getId())
                    cvManagePlayerTurn.wait(lock);

                write(player->getSocket(), message.c_str(), message.size() + 1);

                int buffLen = 100;
                char buffer [buffLen + 1];
                buffer[buffLen] = '\0';
                bzero(buffer, buffLen);
                read(player->getSocket(), buffer, buffLen);

                std::string receivedMsg = buffer;
                if (!receivedMsg.empty()) {
                    receivedMsg.pop_back();
                }
                std::string parsedMsg = receivedMsg.substr(receivedMsg.find({'%', '%'}) + 1);

                // update board
                board.movePawn(player->getId(), '0', 6);

                manPlayerTurns = false;
                // loop player id on turn from 1 to player count
                playerIdOnTurn = playerIdOnTurn + 1 > players->size() ? 1 : playerIdOnTurn + 1;
                updBoard = true;
                cvUpdateBoard.notify_one();
            }

        }
    }
}