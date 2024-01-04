#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        try {
            std::cout << "Connecting to " << ipaddr << "...\n";
            socket = MySocket::createConnection(ipaddr, port);
            std::cout << "Successfully connected to " << ipaddr << "\n";
            WaitForStart();
        }
        catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
    }

    void ClientGame::WaitForStart() {
        std::cout << "Waiting for game to start...\n";
        std::string data = socket->receiveData();
    }

    ClientGame::~ClientGame()
    {
        delete socket;
    }
}
