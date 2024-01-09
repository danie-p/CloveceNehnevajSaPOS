#include <iostream>

#include "ClientGame.h"

int main()
{
    using namespace Client;


    std::cout << "**** Clovece Nehnevaj Sa ****\n\n";
    std::cout << "Welcome to the game (client)\n";
    std::cout << "You can use this client app to join an existing game.\n";
    std::cout << "To create a new game, use the server app!\n";
    std::cout << "*****************************************\n\n";

    std::cout << "Enter ip address and port of game server you wish to connect to\n";
    std::cout << "In the format 'ipaddress:port'\n";
    std::cout << "or type 'exit' to quit the application:\n";
    std::cout << "Input: ";
    std::string input;
    std::cin >> input;

    std::string ipaddr = "";
    int port = 0;
    ClientGame cGame;

    try {
        ipaddr = input.substr(0, input.find(":"));
        port = std::stoi(input.substr(input.find(":") + 1));
        cGame.Connect(ipaddr, port);
    }
    catch (std::exception& e) {
        std::cout << "Invalid input...\n";
    }

    return 0;
}


