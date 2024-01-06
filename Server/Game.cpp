//
// Created by Kristian on 3. 1. 2024.
//

#include <csignal>
#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        char colors[] = {'R', 'G', 'B', 'Y'};
        for (int i = 0; i < clientSockets.size(); ++i) {
            players.push_back(new Player(i + 1, clientSockets[i], 1, colors[i]));
        }
        std::cout << "Game instance created\n";
        this->board.initializeGrid();
    }

    Game::~Game() {
        End();
        std::cout << "Game instance destroyed\n";
    }

    void Game::Begin() {
        std::cout << "Game starting..\n";
        sendUpdateThread = new std::thread(&Game::SendUpdate, this);
        manageTurnThread = new std::thread(&Game::ManagePlayerTurn, this);
    }

    void Game::End() {
        if (manageTurnThread != nullptr && manageTurnThread->joinable()) {
            manageTurnThread->join();
            delete manageTurnThread;
            manageTurnThread = nullptr;
        }

        if (sendUpdateThread != nullptr && sendUpdateThread->joinable()) {
            sendUpdateThread->join();
            delete sendUpdateThread;
            sendUpdateThread = nullptr;
        }

        for (auto & player : players) {
            delete player;
            player = nullptr;
        }
    }

    void Game::SendUpdate() {
        while (!gameOver) {
            for (auto& player : players) {
                std::string message = "";
                message += std::to_string(player->getId());
                message += END_MESSAGE;
                {
                    std::unique_lock<std::mutex> lock(dataLock);
                    if (!turnManaged)
                        cvSendUpdate.wait(lock);

                    message += board.toString();
                    message += END_MESSAGE;

                    if (!gameOver) {
                        message += std::to_string(playerIdOnTurn);
                        message += END_MESSAGE;
                    }
                    else {
                        message += std::to_string(board.getWinner()->getId());
                        message += END_MESSAGE;
                    }

                    message += board.getMessages();
                    message += END_MESSAGE;

                    updateSent = true;
                    turnManaged = false;
                    cvManagePlayerTurn.notify_one();
                }

                std::cout << "Sending id, board and id on turn to client " << player->getId() << "...\n";
                for (auto& player : players) {
                    message.at(0) = char(player->getId() + '0');
                    write(player->getSocket(), message.c_str(), message.size() + 1);
                }

                if (gameOver)
                    break;
            }
        }
    }

    // must wait for work done by the SendUpdate thread
    void Game::ManagePlayerTurn() {
        while (!gameOver) {
            for (auto& player: players) {

                int buffLen = 4096;
                char buffer[buffLen];
                bzero(buffer, buffLen);

                std::vector<std::string> *messages = new std::vector<std::string>();
                int msgCount = 0;

                std::cout << "Reading input from client " << player->getId() << "...\n";
                while (msgCount < 2) { // receive 2 messages: 1. throw; 2. pawn number
                    std::string receivedMsg = "";
                    while (receivedMsg.find(END_MESSAGE) == std::string::npos) {
                        read(player->getSocket(), buffer, buffLen);
                        receivedMsg = buffer;
                        // the server will only receive message from player at the end of the turn,
                        // with the turn lasting a few seconds
                        // therefore we can sleep for a few seconds instead of checking the condition over and over again
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }

                    int index = 0;
                    while ((index = receivedMsg.find(END_MESSAGE)) != std::string::npos) {
                        messages->push_back(receivedMsg.substr(0, index));
                        receivedMsg = receivedMsg.substr(index + 1);
                        msgCount++;
                    }
                }

                {
                    std::unique_lock<std::mutex> lock(dataLock);
                    if (!updateSent)
                        cvManagePlayerTurn.wait(lock);

                    int newIdOnTurn = playerIdOnTurn + 1;
                    if (newIdOnTurn > players.size())
                        newIdOnTurn = 1;

                    playerIdOnTurn = newIdOnTurn;

                    int numThrown = std::stoi(messages->at(0));
                    char pawnNum = messages->at(1).c_str()[0];

                    board.movePawn(player->getId(), pawnNum, numThrown);

                    if (board.isGameOver())
                        gameOver = true;

                    turnManaged = true;
                    updateSent = false;
                    std::cout << "Client " << player->getId() << " finished his turn\n";
                    cvSendUpdate.notify_one();

                    if (gameOver)
                        break;
                }

                delete messages;
            }
        }
    }
}