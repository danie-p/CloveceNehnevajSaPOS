//
// Created by Kristian on 3. 1. 2024.
//

#include "ServerConnection.h"

void Server::Create(int port) {
    if (port <= 0) {
        std::cout << "Invalid port! (value: " << port << ")\n";
        return;
    }

    int serverPassiveSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverPassiveSocket < 0) {
        std::cout << "Error creating server passive socket\n";
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverPassiveSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Error binding server passive socket\n";
        return;
    }

    std::cout << "Successfully create game server\n";

    std::cout << "Waiting for players to connect...\n";
    if (listen(serverPassiveSocket, 10) < 0) {
        std::cout << "Error in listening for connections\n";
    }

    sockaddr_in clientAddresses[PLAYER_COUNT];
    int clientSockets[PLAYER_COUNT];

    for (int i = 0; i < PLAYER_COUNT; ++i) {
        socklen_t clientAddrSize = sizeof(clientAddresses[i]);
        clientSockets[i] = accept(serverPassiveSocket, (sockaddr*)&clientAddresses[i], &clientAddrSize);
        if (clientSockets[i] <= 0) {
            std::cout << "Error in accepting connection\n";
            return;
        }

        std::cout << "Player connected to server.\n";
    }

    close(serverPassiveSocket);
    std::cout << "All players connected, starting game...\n";

    // start game
    PlayGame(clientSockets);
    // end game

    for (int i = 0; i < PLAYER_COUNT; ++i) {
        close(clientSockets[i]);
    }
}

void Server::PlayGame(int clientSockets[]) {
    std::vector<int> sockets;
    for (int i = 0; i < PLAYER_COUNT; ++i) {
        sockets.push_back(clientSockets[i]);
    }

    Game game(sockets);
    game.Begin();
    game.End();
}
