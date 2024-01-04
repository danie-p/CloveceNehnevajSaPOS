#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        try {
            std::cout << "Connecting to " << ipaddr << "...\n";
            socket = MySocket::createConnection(ipaddr, port);
            std::cout << "Successfully conencted to " << ipaddr << "\n";

            // simulate sending message for debugging purposes
            std::this_thread::sleep_for(std::chrono::seconds(30));
            socket->sendData("testing data hello can you see me");
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
