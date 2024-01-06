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
        std::vector<std::string> *data;

        while (!gameOver) {
            std::cout << "Waiting for turn...\n";

            data = socket->receiveData(4);

            if (1 == turn)
                playerId = std::stoi(data->at(0));

            std::cout << "Turn " << turn << "\n";
            std::cout << "Updated board:\n";
            std::cout << data->at(1);
            std::cout << "Last turn: " << (data->at(3).empty() ? "No data\n" : data->at(3)) << "\n";
            YouAreColor(playerId);

            // check if it's game over
            if (data->at(2).find("gameover") != std::string::npos) {
                gameOver = true;
                break;
            }

            if (data->at(2) == std::to_string(playerId)) {
                std::cout << "It is your turn.\n";

                int numThrown = ThrowDice();

                int pawnPicked = 1;
                if (numThrown != 0)
                    pawnPicked = PickPawn();

                system("pause");

                socket->sendData(std::to_string(numThrown));
                socket->sendData(std::to_string(pawnPicked));
            }

            turn++;
        }

        std::cout << "The game is over!\n";
        std::string winnerId = data->at(1).substr(data->at(0).size() - 2, data->at(0).size() - 1);
        std::cout << "Player Id " << winnerId << " wins!\n";

        delete data;
    }

    int ClientGame::ThrowDice() {
        std::uniform_int_distribution<> distrib(1, 6);
        int result = 0;

        if (1 == turn) {
            const int attempts = 5;
            std::cout << "First turn, " << attempts << " attempts to throw 6...\n";
            for (int i = 0; i < attempts; ++i) {
                system("pause");
                result = distrib(gen);
                if (6 == result) {
                    std::cout << "Success! Result of throw is 6\n";
                    return result;
                } else {
                    std::cout << "Result of throw is " << result << ". Bad luck!\n";
                    result = 0;
                }
            }
            std::cout << "Better luck next time!\n";
        }
        else {
            std::cout << "Time to throw the dice...\n";
            system("pause");
            result = distrib(gen);
            std::cout << "Result of throw: " << result << "\n";
        }

        return result;
    }

    ClientGame::~ClientGame() {
        delete socket;
    }

    int ClientGame::PickPawn() {
        std::cout << "Pick which pawn you wish to move: 1, 2, 3, 4 (type anything to pick the first one)";
        std::string input = "";
        int result = 1;
        std::cin >> input;
        if (input == "2")
            result = 2;
        else if (input == "3")
            result = 3;
        else if (input == "4")
            result = 4;

        std::cout << "You picked pawn " << result << "\n";
        return result;
    }

    void ClientGame::YouAreColor(int id) {
        std::cout << "You play as color ";
        switch (id) {
            case 1: std::cout << RED << "RED\n"; break;
            case 2: std::cout << GREEN  << "GREEN\n"; break;
            case 3: std::cout << BLUE  << "BLUE\n"; break;
            case 4: std::cout << YELLOW  << "YELLOW\n"; break;
        }
        std::cout << RESET;
    }
}
