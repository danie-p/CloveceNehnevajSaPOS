#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        try {
            socket = MySocket::createConnection(ipaddr, port);
            WaitForStart();
        }
        catch (std::exception& e) {
            std::cout << "Error connecting to server\n";
        }
    }

    void ClientGame::WaitForStart() {

    }

    ClientGame::~ClientGame()
    {
        delete socket;
    }
}
