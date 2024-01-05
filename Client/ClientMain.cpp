#include <iostream>

#include "ClientGame.h"

int main()
{
    using namespace Client;
    
    ClientGame* cGame = new ClientGame();

    std::cout << "**** Clovece Nehnevaj Sa ****\n\n";
    std::cout << "Welcome to the game (client)\n";
    std::cout << "You can use this client app to join an existing game.\n";
    std::cout << "To create a new game, use the server app!\n";
    std::cout << "*****************************************\n\n";

    std::cout << "Enter ip address of game server you wish to connect to or type 'exit' to quit the application:\n";
    std::cout << "Input: ";
    std::string input;
    std::cin >> input;
    cGame->Connect(input, 1048);

    delete cGame;

    return 0;
}


