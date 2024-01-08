#include <iostream>
#include "ServerConnection.h"

int main() {
    std::cout << "**** Clovece Nehnevaj Sa ****\n\n";
    std::cout << "Welcome to the game (server)\n";
    std::cout << "You can use this server app to create a new game.\n";
    std::cout << "To join an existing game, use the client app!\n";
    std::cout << "*****************************************\n\n";

    std::cout << "Type 'create' to create new game server\n";
    std::cout << "Input: ";
    std::string input;
    std::cin >> input;

    if (input == "create") {
        std::cout << "Enter port to start the game server on:\n";
        std::cout << "Input: ";
        std::string portStr;
        std::cin >> portStr;

        try {
            int port = std::stoi(portStr);
            Server::Create(port);
        }
        catch (std::exception& e) {
            std::cout << "Invalid input...\n";
        }
    }


    return 0;
}
