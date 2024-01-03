#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        socket = MySocket::createConnection(ipaddr, port);
    }

    ClientGame::~ClientGame()
    {
        delete socket;
    }
}
