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

    // Message order:
    // 0: playerId
    // 1: board
    // 2: player on turn / game over message + winner id
    // 3: board messages
    // 4: turn
    void ClientGame::Play() {
        std::vector<std::string> *data;
        std::string msgPlayerId;
        std::string board;
        std::string playerOnTurn;
        std::string boardMessages;
        std::string msgTurn;

        while (!gameOver) {
            std::cout << "Waiting for turn...\n";
            data = socket->receiveData(5);

            msgPlayerId = data->at(0);
            board = data->at(1);
            playerOnTurn = data->at(2);
            boardMessages = "Last turn:\n" + (data->at(3).empty() ? "No data\n" : data->at(3)) + "\n";
            msgTurn = data->at(4);

            turn = std::stoi(msgTurn);

            if (1 == turn)
                playerId = std::stoi(msgPlayerId);

            std::cout << "\nTurn " << turn << "\n";
            std::cout << "Updated board:\n";
            std::cout << board;
            std::cout << boardMessages;
            YouAreColor(playerId);

            // check if it's game over
            if (playerOnTurn.find(GAME_OVER) != std::string::npos) {
                gameOver = true;
                break;
            }

            if (playerOnTurn == std::to_string(playerId)) {
                std::cout << "It is your turn.\n";

                int numThrown = ThrowDice();

                int pawnPicked = 1;
                if (numThrown != 0)
                    pawnPicked = PickPawn();

                system("pause");

                socket->sendData(std::to_string(numThrown));
                socket->sendData(std::to_string(pawnPicked));
            }
        }

        std::cout << "The game is over!\n";
        std::string winnerId = playerOnTurn.substr(playerOnTurn.size() - 2, playerOnTurn.size() - 1);
        // TODO: better identify the winner (by color)
        std::cout << "Player Id " << winnerId << " wins!\n";

        delete data;
    }

    int ClientGame::ThrowDice() {
        int result = 0;

        if (turn >= 1 && turn <= 4) {
            const int attempts = 3;
            std::cout << "First three turns, you have " << attempts << " attempts to throw 6...\n";
            for (int i = 0; i < attempts; ++i) {
                system("pause");
                result = this->distrib(this->gen);
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
        std::cout << "Pick which pawn you wish to move: 1, 2, 3, 4 (type anything to pick the first one): ";
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
