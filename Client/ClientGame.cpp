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
    // 2: player on turn
    // 3: board messages
    // 4: turn
    // 5: game over message
    // 6: winner color
    // 7: player on turn color
    // 8: last disconnected player color
    void ClientGame::Play() {
        std::string winnerColor;

        while (!gameOver) {
            std::cout << "Waiting for turn...\n";
            GameData* data = socket->receiveData(9);

            if (data->playerId == "DISCONNECT") {
                break;
            }

            winnerColor = data->winnerColor;
            turn = std::stoi(data->turn);

            if (1 == turn)
                playerId = std::stoi(data->playerId);

            std::cout << "\nTurn " << turn << "\n";
            YouAreColor(playerId);
            std::cout << "Game Board:\n";
            std::cout << data->board;
            std::cout << "Last turn's events:\n";
            if (data->lastDisconnectedPlayerColor.empty())
                std::cout << data->boardMessages;
            if (!data->lastDisconnectedPlayerColor.empty())
                std::cout << "Player [" << data->lastDisconnectedPlayerColor << "] has disconnected.\n";
            std::cout << "\n";

            // check if it's game over
            if (data->gameOver == GAME_OVER) {
                gameOver = true;
                break;
            }

            int pawnPicked = 1;

            if (data->playerOnTurn == std::to_string(playerId)) {
                std::cout << "It is your turn.\n";

                int numThrown = ThrowDice();

                pawnPicked = 1;
                if (numThrown != 0)
                    pawnPicked = PickPawn();

                system("pause");

                socket->sendData(std::to_string(numThrown));
                socket->sendData(std::to_string(pawnPicked));
            }
            else {
                std::cout << "It is player's " << data->playerOnTurn << " [" << data->playerOnTurnColor << "] turn.\n";
            }

            if (pawnPicked == -1) {
                disconnect = true;
                break;
            }
        }

        if (!disconnect) {
            std::cout << "The game is over!\n";
            std::cout << "Player [" << winnerColor << "] is the first one to place all pawns in their home and wins!\n";
        } else {
            std::cout << "Disconnected.\n";
        }
    }

    int ClientGame::ThrowDice() {
        int result = 0;

        // turn is incremented every time any of the players gets to make a move
        // since we have 4 players, 1 round of the game consists of 4 turns of the players
        // we want to let all 4 players have a chance to roll the dice 3x in the first round
        if (1 <= turn && turn <= 4) {
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
        std::cout << "Pick which pawn you wish to move: 1, 2, 3, 4 (type anything to pick pawn 1) or type 'exit' to disconnect: ";
        std::string input = "";
        int result = 1;
        std::cin >> input;
        if (input == "2")
            result = 2;
        else if (input == "3")
            result = 3;
        else if (input == "4")
            result = 4;
        else if (input == "exit")
            result = -1;

        if (result != -1)
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
