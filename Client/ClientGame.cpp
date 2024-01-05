#include "ClientGame.h"

namespace Client
{
    void ClientGame::Connect(std::string ipaddr, unsigned int port)
    {
        try {
            std::cout << "Connecting to " << ipaddr << "...\n";
            socket = MySocket::createConnection(ipaddr, port);
            std::cout << "Successfully connected to " << ipaddr << "\n";
            Play();
        }
        catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
    }

    void ClientGame::Play() {
        std::cout << "Waiting for turn...\n";

        int numOfMessagesToWaitFor = turn == 1 ? 3 : 2;
        std::vector<std::string>* data = socket->receiveData(numOfMessagesToWaitFor);

        std::cout << "It is your turn!\n";

        if (numOfMessagesToWaitFor == 3) {
            playerId = std::stoi(data->at(0));
            data->erase(data->begin());
        }

        std::cout << "Updated board:\n";
        std::cout << data->at(0) << "\n";

        if (data->size() == 2) {
            if (data->at(1) == std::to_string(playerId)) { // it is your turn
                std::cout << "It is your turn.\n";

                // hod kockou
                // vyber panacika
                // cakaj na potvrdenie
                // posli hod a panacika serveru...
            }
        }

        delete data;
    }

    ClientGame::~ClientGame()
    {
        delete socket;
    }
}
