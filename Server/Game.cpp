//
// Created by Kristian on 3. 1. 2024.
//

#include "Game.h"

namespace Server {

    Game::Game(std::vector<int> clientSockets) {
        char colors[] = {'R', 'G', 'B', 'Y'};
        for (int i = 0; i < clientSockets.size(); ++i) {
            players.push_back(new Player(i + 1, clientSockets[i], colors[i]));
        }
        std::cout << "Game instance created\n";
        this->board.initializeGrid();
    }

    void Game::DisconnectAll() {
        stopThreads = true;
        for (auto& player : players) {
            std::string msg = "DISCONNECT%";
            for (int i = 0; i < 8; ++i) {
                write(player->getSocket(), msg.c_str(), msg.size() + 1);
            }
        }
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

    // Message order:
    // 0: playerId
    // 1: board
    // 2: player on turn
    // 3: board messages
    // 4: turn
    // 5: game over message
    // 6: winner color
    // 7: player on turn color
    // 8: last disconnected player color
    void Game::SendUpdate() {
        while (!gameOver) {
            if (stopThreads) {
                return;
            }

            std::list<std::string> messages;

            {
                std::unique_lock<std::mutex> lock(dataLock);
                if (!turnManaged)
                    cvSendUpdate.wait(lock);

                messages.push_back(board.toString());
                messages.push_back(std::to_string(playerIdOnTurn));
                messages.push_back(board.getMessages());
                messages.push_back(std::to_string(turn));

                if (gameOver && numOfDisconnectedPlayers != players.size()) {
                    messages.push_back(GAME_OVER);
                    messages.push_back(board.getWinner()->getFullColor());
                } else {
                    messages.emplace_back("");
                    messages.emplace_back("");
                }

                messages.push_back(players[playerIdOnTurn - 1]->getFullColor());
                messages.push_back(this->lastDisconnectedPlayerColor);

                updateSent = true;
                turnManaged = false;
                cvManagePlayerTurn.notify_one();
            }

            std::cout << "Sending id, board and id on turn to clients...\n";
            for (auto& player : players) {
                if (!player->getDisconnected()) {
                    messages.push_front(std::to_string(player->getId()));

                    std::string finalMessage = "";
                    for (const auto &message: messages) {
                        finalMessage += message;
                        finalMessage += END_MESSAGE;
                    }

                    write(player->getSocket(), finalMessage.c_str(), finalMessage.size() + 1);
                    messages.pop_front();
                }
            }

            if (gameOver)
                break;
        }
    }

    // Receive message order:
    // 0: number thrown / disconnect message
    // 1: pawn to be moved / disconnect message
    // must wait for work done by the SendUpdate thread
    void Game::ManagePlayerTurn() {
        while (!gameOver) {
            if (stopThreads) {
                return;
            }

            for (auto& player: players) {
                if (!player->getDisconnected()) {
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

                        int newIdOnTurn = playerIdOnTurn % players.size() + 1;
                        while (players.at(newIdOnTurn - 1)->getDisconnected()) {
                            newIdOnTurn = newIdOnTurn % players.size() + 1;
                        }

                        playerIdOnTurn = newIdOnTurn;

                        int numThrown = std::stoi(messages->at(0));
                        char pawnNum = messages->at(1).c_str()[0];

                        if (pawnNum == '-') {
                            std::cout << "Client " << player->getId() << " has requested to disconnect.\n";
                            player->setDisconnected(true);
                            this->numOfDisconnectedPlayers++;
                            this->lastDisconnectedPlayerColor = player->getFullColor();
                        } else {
                            this->lastDisconnectedPlayerColor = "";
                            board.movePawn(player->getId(), pawnNum, numThrown);
                        }

                        turn++;

                        if (board.isGameOver() || this->numOfDisconnectedPlayers == this->players.size())
                            gameOver = true;

                        turnManaged = true;
                        updateSent = false;
                        std::cout << "Client " << player->getId() << " finished his turn\n";

                        cvSendUpdate.notify_one();

                        delete messages;

                        if (gameOver)
                            break;
                    }
                }
            }
        }
    }
}