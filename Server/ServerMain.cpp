#include <iostream>
#include "ServerGame.h"

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
    if (input == "create")
        Server::Create(1048);


    return 0;
}
