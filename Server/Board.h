#pragma once

#include <vector>
#include <sstream>
#include "Square.h"

namespace Server
{
    class Board
    {
    public:
        Board(std::vector<Player*>* players);
        ~Board();
        std::string toString();
        void display();
        bool movePawn(int playerId, char pawnNum, int moveSteps);	// move playerId's pawnNum-th pawn by moveSteps steps
        bool isGameOver();	// check if game is over
        Player* getWinner();	// get winner id
        std::string getMessages() { return this->messages.str(); };

        Server::Square& getSquare(int rowIndex, int columnIndex)
        {
            return this->grid.at(rowIndex).at(columnIndex);
        }

        Server::Square* getSquareWithPlayersPawn(int playerId, int pawnNum)
        {
            for (auto& row : this->grid)
            {
                for (auto& square : row) {
                    if (square.getPawn()) {
                        if (square.getPawn()->player->getId() == playerId && square.getPawn()->number == pawnNum) {
                            return &square;
                        }
                    }
                }
            }
            return nullptr;
        }

        void initializeGrid();

    private:
        static const int GRID_ROWS = 11;
        static const int GRID_COLUMNS = 11;
        std::vector<std::vector<Square>> grid;

        std::vector<Square*> path;
        void reorderPath();

        std::vector<Square*> startR;
        std::vector<Square*> startG;
        std::vector<Square*> startB;
        std::vector<Square*> startY;
        std::vector<Square*> homeR;
        std::vector<Square*> homeG;
        void reorderHomeG();
        std::vector<Square*> homeB;
        std::vector<Square*> homeY;
        void reorderHomeY();
        std::vector<std::vector<Square*>*> homes;

        bool isInStart(Pawn& pawn, int& indexInStart);
        bool isAtHome(Pawn& pawn, int& indexAtHome);
        bool isOnPath(Pawn& pawn, int& indexOnPath);
        void getInitialPathSquareCoords(Pawn& pawn, int& row, int& column);
        void tryToGoHome(std::vector<Square*>* home, int indexHome, Pawn& pawn, Square& oldSquare);
        bool movePawnOut(Square& newSquare, Pawn& pawn);

        std::vector<Player*>* players;
        Player* winner = nullptr;
        std::stringstream messages;
    };
}