#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        try {
            std::cout << "Connecting to " << ipaddr << "...\n";
            socket = MySocket::createConnection(ipaddr, port);
            std::cout << "Successfully conencted to " << ipaddr << "\n";
        }
        catch (std::exception& e) {
            std::cout << "Error connecting to server: " << e.what() << "\n";
        }
    }

    void ClientGame::WaitForStart() {

    }

    ClientGame::~ClientGame()
    {
        delete socket;
    }
}
