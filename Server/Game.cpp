//
// Created by Kristian on 3. 1. 2024.
//

#include <csignal>
#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        char colors[] = {'R', 'G', 'B', 'Y'};
        this->players = new std::vector<Player*>();
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

        delete players;
        players = nullptr;

        std::cout << "Game instance destroyed\n";
    }

    void Game::Begin() {
        std::cout << "Game starting..\n";

        for (int i = 0; i < players->size(); ++i)
        {
            playerThreads.push_back(new std::thread(&Game::ManagePlayerTurn, this, players->at(i)));
            std::cout << "Created turn management thread for player id " << players->at(i)->getId() << "\n";
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
        //signal(SIGPIPE, SIG_IGN);

        // first send ids to clients
        if (!idsSent) {
            std::cout << "Sending player ids to clients...\n";
            for (auto & player : *players) {
                std::string message = "$id:";
                std::string id = std::to_string(player->getId());
                message += id;
                message += END_MESSAGE;
                {
                    std::unique_lock<std::mutex> lock(*player->getMutex());
                    while (!updBoard) {
                        cvUpdateBoard.wait(lock);
                    }
                    write(player->getSocket(), message.c_str(), message.size() + 1);
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

                std::string receivedMsg = "";
                while (receivedMsg.find(END_MESSAGE) == std::string::npos) {
                    read(player->getSocket(), buffer, buffLen);
                    receivedMsg = buffer;
                    // it can be assumed that any reply from client when it's his turn will take him
                    // at least a second or two to send (reply has to be sent manually by pressing Enter for instance)
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                
                if (!receivedMsg.empty()) {
                    receivedMsg.pop_back();
                }
                


                // update board


                manPlayerTurns = false;
                // loop player id on turn from 1 to player count
                playerIdOnTurn = playerIdOnTurn + 1 > players->size() ? 1 : playerIdOnTurn + 1;
                updBoard = true;
                cvUpdateBoard.notify_one();
            }

        }
    }
}