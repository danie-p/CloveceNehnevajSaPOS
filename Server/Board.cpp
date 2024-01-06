#include "Board.h"
#include <iostream>
#include <algorithm>

Server::Board::Board(std::vector<Player*>* players) :
        grid(GRID_ROWS, std::vector<Square>(GRID_COLUMNS)),
        players(players)
{
    this->homes.push_back(&homeR);
    this->homes.push_back(&homeG);
    this->homes.push_back(&homeB);
    this->homes.push_back(&homeY);
}

Server::Board::~Board() {
    if (players != nullptr) {
        for (int i = 0; i < players->size(); ++i) {
            if (players->at(i) != nullptr) {
                delete players->at(i);
                players->at(i) = nullptr;
            }
        }

        delete players;
        players = nullptr;
    }
}

void Server::Board::initializeGrid()
{
    char counterR = '1';
    char counterG = '1';
    char counterB = '1';
    char counterY = '1';

    for (size_t i = 0; i < GRID_ROWS; i++)
    {
        for (size_t j = 0; j < GRID_COLUMNS; j++)
        {
            Square& square = this->grid.at(i).at(j);

            // start red
            if (i == 0 && j == 0 || i == 0 && j == 1 ||
                i == 1 && j == 0 || i == 1 && j == 1)
            {
                square.setSquareType(SquareType::StartR);
                square.setEmpty(false);
                square.setPawn(new Pawn(counterR, this->players->at(0)));
                counterR++;
                this->startR.push_back(&square);
            }

            // start blue
            if (i == 0 && j == GRID_COLUMNS - 2 || i == 0 && j == GRID_COLUMNS - 1 ||
                i == 1 && j == GRID_COLUMNS - 2 || i == 1 && j == GRID_COLUMNS - 1)
            {
                square.setSquareType(SquareType::StartB);
                square.setEmpty(false);
                square.setPawn(new Pawn(counterB, this->players->at(2)));
                counterB++;
                this->startB.push_back(&square);
            }

            // start yellow
            if (i == GRID_ROWS - 2 && j == 0 || i == GRID_ROWS - 2 && j == 1 ||
                i == GRID_ROWS - 1 && j == 0 || i == GRID_ROWS - 1 && j == 1)
            {
                square.setSquareType(SquareType::StartY);
                square.setEmpty(false);
                square.setPawn(new Pawn(counterY, this->players->at(3)));
                counterY++;
                this->startY.push_back(&square);
            }

            // start green
            if (i == GRID_ROWS - 2 && j == GRID_COLUMNS - 2 || i == GRID_ROWS - 2 && j == GRID_COLUMNS - 1 ||
                i == GRID_ROWS - 1 && j == GRID_COLUMNS - 2 || i == GRID_ROWS - 1 && j == GRID_COLUMNS - 1)
            {
                square.setSquareType(SquareType::StartG);
                square.setEmpty(false);
                square.setPawn(new Pawn(counterG, this->players->at(1)));
                counterG++;
                this->startG.push_back(&square);
            }

            // home blue
            if (i >= 1 && i <= 4 && j == 5)
            {
                square.setSquareType(SquareType::HomeB);
                this->homeB.push_back(&square);
            }

            // home yellow
            if (i >= 6 && i <= 9 && j == 5)
            {
                square.setSquareType(SquareType::HomeY);
                this->homeY.push_back(&square);
            }

            // home red
            if (i == 5 && j >= 1 && j <= 4)
            {
                square.setSquareType(SquareType::HomeR);
                this->homeR.push_back(&square);
            }

            // home green
            if (i == 5 && j >= 6 && j <= 9)
            {
                square.setSquareType(SquareType::HomeG);
                this->homeG.push_back(&square);
            }

            if (i != 5 && j == 4 || i != 5 && j == 6 || i == 4 && j != 5 || i == 6 && j != 5 ||
                i == 0 && j == 5 || i == GRID_ROWS - 1 && j == 5 || i == 5 && j == 0 || i == 5 && j == GRID_COLUMNS - 1)
            {
                square.setSquareType(SquareType::Path);
                this->path.push_back(&square);
            }

            if (i == 4 && j == 0) square.setSquareType(SquareType::PathR);
            if (i == 0 && j == 6) square.setSquareType(SquareType::PathB);
            if (i == 6 && j == GRID_COLUMNS - 1) square.setSquareType(SquareType::PathG);
            if (i == GRID_ROWS - 1 && j == 4) square.setSquareType(SquareType::PathY);
        }
    }

    this->reorderPath();
    this->reorderHomeG();
    this->reorderHomeY();
}

void Server::Board::display()
{
    std::cout << this->toString();
}

bool Server::Board::movePawn(int playerId, char pawnNum, int moveSteps)
{
    this->messages.str("");
    this->messages.clear();

    Square* oldSquare = this->getSquareWithPlayersPawn(playerId, pawnNum);
    if (oldSquare == nullptr)
        return false;

    Pawn* pawnPtr = oldSquare->getPawn();
    if (pawnPtr == nullptr)
        return false;

    Pawn& pawn = *pawnPtr;

    int index = -1;

    // === START ===
    if (this->isInStart(pawn, index)) // tu index nepotrebujem
    {
        if (moveSteps >= 6)
        {
            int row;
            int column;
            this->getInitialPathSquareCoords(pawn, row, column);
            Square& newSquare = this->getSquare(row, column);

            if (!newSquare.isEmpty())
            {
                if (!this->movePawnOut(newSquare, pawn)) return true;
            }

            pawn.isInStart = false;
            pawn.isOnPath = true;
            oldSquare->setEmpty(true);
            oldSquare->setPawn(nullptr);
            newSquare.setEmpty(false);
            newSquare.setPawn(&pawn);
            this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] moved pawn " << pawnNum << " from start to path.\n";
            std::cout << this->messages.str();
            return true;
        }
        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " out of start because they didn't roll a 6.\n";
        std::cout << this->messages.str();
        return true;
    }

    // === PATH ===
    if (this->isOnPath(pawn, index))
    {
        switch (pawn.player->getColor())
        {
            case 'R':
                if (index < 32 && index + moveSteps >= 36)
                {
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " beyond its starting place.\n";
                    std::cout << this->messages.str();
                    return true;
                }
                if (!(index >= 32 && index <= 35) && index + moveSteps >= 32 && index + moveSteps < 36)
                {
                    int indexHome = (index + moveSteps) % 30 - 2;
                    this->tryToGoHome(&this->homeR, indexHome, pawn, *oldSquare);
                    return true;
                }
                break;
            case 'G':
                if (index < 12 && index + moveSteps >= 16)
                {
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " beyond its starting place.\n";
                    std::cout << this->messages.str();
                    return true;
                }
                if (!(index >= 12 && index <= 15) && index + moveSteps >= 12 && index + moveSteps < 16)
                {
                    int indexHome = (index + moveSteps) % 10 - 2;
                    this->tryToGoHome(&this->homeG, indexHome, pawn, *oldSquare);
                    return true;
                }
                break;
            case 'B':
                if (index < 2 && index + moveSteps >= 6)
                {
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " beyond its starting place.\n";
                    std::cout << this->messages.str();
                    return true;
                }
                if (!(index >= 2 && index <= 5) && (index + moveSteps) % 40 >= 2 && (index + moveSteps) % 40 < 6)
                {
                    int indexHome = (index + moveSteps) % 40 - 2;
                    this->tryToGoHome(&this->homeB, indexHome, pawn, *oldSquare);
                    return true;
                }
                break;
            case 'Y':
                if (index < 22 && index + moveSteps >= 26)
                {
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " beyond its starting place.\n";
                    std::cout << this->messages.str();
                    return true;
                }
                if (!(index >= 22 && index <= 25) && index + moveSteps >= 22 && index + moveSteps < 26)
                {
                    int indexHome = (index + moveSteps) % 20 - 2;
                    this->tryToGoHome(&this->homeY, indexHome, pawn, *oldSquare);
                    return true;
                }
                break;
            default:
                break;
        }

        Square& newSquare = *this->path.at((index + moveSteps) % 40);

        if (!newSquare.isEmpty())
        {
            if (!this->movePawnOut(newSquare, pawn)) return true;
        }

        oldSquare->setEmpty(true);
        oldSquare->setPawn(nullptr);
        newSquare.setEmpty(false);
        newSquare.setPawn(&pawn);

        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] moved pawn " << pawnNum << " along the path.\n";
        std::cout << this->messages.str();
        return true;
    }

    // === HOME ===
    if (this->isAtHome(pawn, index)) {
        if (index + moveSteps <= 3) {
            std::vector<Square*>* home = nullptr;
            switch (pawn.player->getColor()) {
                case 'R':
                    home = &homeR;
                    break;
                case 'G':
                    home = &homeG;
                    break;
                case 'B':
                    home = &homeB;
                    break;
                case 'Y':
                    home = &homeY;
                    break;
            }

            if (home) {
                Square& newSquare = *(*home).at(index + moveSteps);
                if (newSquare.isEmpty()) {
                    oldSquare->setEmpty(true);
                    oldSquare->setPawn(nullptr);
                    newSquare.setEmpty(false);
                    newSquare.setPawn(&pawn);
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] moved pawn " << pawnNum << " further inside the home.\n";
                    std::cout << this->messages.str();
                } else {
                    this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " further inside the home because the place is already occupied.\n";
                    std::cout << this->messages.str();
                }
            }
            return true;
        }
        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " further inside the home.\n";
        std::cout << this->messages.str();
        return true;
    }

    return false;
}

bool Server::Board::isGameOver()
{
    for (auto home : this->homes) {
        bool homeIsFull = true;
        for (auto square : *home) {
            if (square->isEmpty()) {
                homeIsFull = false;
            }
        }
        if (homeIsFull) {
            this->winner = home->at(0)->getPawn()->player;
            return true;
        }
    }

    return false;
}

/*
 * only call when game is over, otherwise nullptr will be returned
 */
Server::Player* Server::Board::getWinner()
{
    return this->winner;
}

// reorders the path vector so that square indices are placed in order of gameplay
void Server::Board::reorderPath()
{
    std::swap(this->path.at(3), this->path.at(4));
    std::swap(this->path.at(4), this->path.at(6));
    std::swap(this->path.at(5), this->path.at(8));
    std::swap(this->path.at(6), this->path.at(14));
    std::swap(this->path.at(7), this->path.at(15));
    std::swap(this->path.at(8), this->path.at(16));
    std::swap(this->path.at(9), this->path.at(17));
    std::swap(this->path.at(10), this->path.at(18));
    std::swap(this->path.at(11), this->path.at(20));
    std::swap(this->path.at(12), this->path.at(30));
    std::swap(this->path.at(13), this->path.at(29));
    std::swap(this->path.at(14), this->path.at(28));
    std::swap(this->path.at(15), this->path.at(27));
    std::swap(this->path.at(16), this->path.at(26));
    std::swap(this->path.at(17), this->path.at(32));
    std::swap(this->path.at(18), this->path.at(34));
    std::swap(this->path.at(19), this->path.at(36));
    std::swap(this->path.at(20), this->path.at(39));
    std::swap(this->path.at(21), this->path.at(38));
    std::swap(this->path.at(22), this->path.at(37));
    std::swap(this->path.at(23), this->path.at(35));
    std::swap(this->path.at(24), this->path.at(33));
    std::swap(this->path.at(25), this->path.at(31));
    std::swap(this->path.at(26), this->path.at(31));
    std::swap(this->path.at(27), this->path.at(33));
    std::swap(this->path.at(28), this->path.at(35));
    std::swap(this->path.at(29), this->path.at(37));
    std::swap(this->path.at(30), this->path.at(38));
    std::swap(this->path.at(31), this->path.at(36));
    std::swap(this->path.at(33), this->path.at(34));
    std::swap(this->path.at(34), this->path.at(39));
    std::swap(this->path.at(35), this->path.at(38));
    std::swap(this->path.at(36), this->path.at(37));
    std::swap(this->path.at(37), this->path.at(39));
    std::swap(this->path.at(38), this->path.at(39));
}

void Server::Board::reorderHomeG()
{
    std::reverse(this->homeG.begin(), this->homeG.end());
}

void Server::Board::reorderHomeY()
{
    std::reverse(this->homeY.begin(), this->homeY.end());
}

bool Server::Board::isInStart(Pawn& pawn, int& indexInStart)
{
    if (pawn.isInStart)
    {
        std::vector<Square*>* start = nullptr;
        switch (pawn.player->getColor())
        {
            case 'R':
                start = &this->startR;
                break;
            case 'G':
                start = &this->startG;
                break;
            case 'B':
                start = &this->startB;
                break;
            case 'Y':
                start = &this->startY;
                break;
        }

        if (start)
        {
            for (size_t i = 0; i < start->size(); i++)
            {
                if (start->at(i)->getPawn() == &pawn)
                {
                    indexInStart = i;
                }
            }
            return true;
        }
    }

    return false;
}

bool Server::Board::isAtHome(Pawn& pawn, int& indexAtHome)
{
    int index = -1;
    if (pawn.isAtHome)
    {
        std::vector<Square*>* home = nullptr;
        switch (pawn.player->getColor())
        {
            case 'R':
                home = &this->homeR;
                break;
            case 'G':
                home = &this->homeG;
                break;
            case 'B':
                home = &this->homeB;
                break;
            case 'Y':
                home = &this->homeY;
                break;
        }

        if (home)
        {
            for (size_t i = 0; i < home->size(); i++)
            {
                if (home->at(i)->getPawn() == &pawn)
                {
                    indexAtHome = i;
                }
            }
            return true;
        }
    }

    return false;
}

bool Server::Board::isOnPath(Pawn& pawn, int& indexOnPath)
{
    int index = -1;
    if (pawn.isOnPath)
    {
        for (size_t i = 0; i < this->path.size(); i++)
        {
            if (this->path.at(i)->getPawn() == &pawn)
            {
                indexOnPath = i;
            }
        }
        return true;
    }

    return false;
}

void Server::Board::getInitialPathSquareCoords(Pawn& pawn, int& row, int& column)
{
    switch (pawn.player->getColor())
    {
        case 'R':
            row = 4;
            column = 0;
            break;
        case 'G':
            row = 6;
            column = GRID_COLUMNS - 1;
            break;
        case 'B':
            row = 0;
            column = 6;
            break;
        case 'Y':
            row = GRID_ROWS - 1;
            column = 4;
            break;
    }
}

void Server::Board::tryToGoHome(std::vector<Square*>* home, int indexHome, Pawn& pawn, Square& oldSquare)
{
    char pawnNum = pawn.number;
    Square* squareHome = (*home).at(indexHome);
    if (squareHome->isEmpty())
    {
        // miesto v domceku je prazdne, moze tam ist
        squareHome->setEmpty(false);
        squareHome->setPawn(&pawn);
        pawn.isOnPath = false;
        pawn.isAtHome = true;
        oldSquare.setEmpty(true);
        oldSquare.setPawn(nullptr);
        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] moved pawn " << pawnNum << " from path to home.\n";
        std::cout << this->messages.str();
    }
    else
    {
        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't move pawn " << pawnNum << " to home because the place is already occupied.\n";
        std::cout << this->messages.str();
    }
}

bool Server::Board::movePawnOut(Square& newSquare, Pawn& pawn)
{
    Pawn* pawnOut = newSquare.getPawn();

    if (pawn.player == pawnOut->player)
    {
        this->messages << "Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "] couldn't make the move and kick out their own pawn.\n";
        std::cout << this->messages.str();
        return false;
    }
    else
    {
        int indexInStart = pawnOut->number - '0' - 1;
        pawnOut->isInStart = true;
        pawnOut->isOnPath = false;

        std::vector<Square *> *start = nullptr;
        switch (pawnOut->player->getColor()) {
            case 'R':
                start = &this->startR;
                break;
            case 'G':
                start = &this->startG;
                break;
            case 'B':
                start = &this->startB;
                break;
            case 'Y':
                start = &this->startY;
                break;
        }

        if (start) {
            Square &pawnOutSquare = *start->at(indexInStart);
            pawnOutSquare.setEmpty(false);
            pawnOutSquare.setPawn(pawnOut);
        }

        this->messages << "Sorry! Player " << pawn.player->getId() << " [" << pawn.player->getFullColor() << "]\'s pawn " << pawn.number << " kicked out Player " << pawnOut->player->getId() << " [" << pawnOut->player->getFullColor() << "]\'s pawn " << pawnOut->number << " from path to start.\n";
        std::cout << this->messages.str();
    }
    return true;
}

std::string Server::Board::toString() {
    std::string result;
    result += "-------------------------\n";
    for (auto& row : this->grid)
    {
        result += "| ";
        for (auto& square : row)
        {
            result += square.toString();
        }
        result += "|\n";
    }
    result += "-------------------------\n";

    return result;
}


