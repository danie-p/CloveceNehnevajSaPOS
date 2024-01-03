#include "Board.h"
#include <iostream>
#include <algorithm>

Common::Board::Board() :
        grid(GRID_ROWS, std::vector<Square>(GRID_COLUMNS)),
        gameOver(false),
        winner(-1)
{
    this->initializeGrid();
    this->reorderPath();
    this->reorderHomeG();
    this->reorderHomeY();
}

void Common::Board::display()
{
    std::cout << "-------------------------\n";
    for (auto& row : this->grid)
    {
        std::cout << "| ";
        for (auto& square : row)
        {
            square.display();
        }
        std::cout << "|\n";
    }
    std::cout << "-------------------------\n";
}

bool Common::Board::movePawn(int playerId, int pawnNum, int moveSteps)
{
    Square& oldSquare = this->getSquareWithPlayersPawn(playerId, pawnNum);
    Pawn& pawn = *oldSquare.getPawn();

    int index = -1;

    // ak je v starte
    // len ak moveSteps == 6, tak sa pohne zo startu na vychodzi bod na path (isInStart = false, isOnPath = true, square stary isEmpty = true, square novy isEmpty = false)
    // vstupom na board moze niekoho vyhodit (kto stoji na jeho vychodziom bode)
    // vyhodenie
    // inak sa nic nestane

    if (this->isInStart(pawn, index)) // tu index nepotrebujem
    {
        if (moveSteps == 6)
        {
            pawn.isInStart = false;
            pawn.isOnPath = true;
            oldSquare.setEmpty(true);
            oldSquare.setPawn(nullptr);
            int row;
            int column;
            this->getInitialPathSquareCoords(pawn, row, column);
            Square& newSquare = this->getSquare(row, column);

            if (!newSquare.isEmpty())
            {
                // vyhodenie
                Pawn* pawnOut = newSquare.getPawn();
                int indexInStart = pawnOut->number - 1;
                pawnOut->isInStart = true;
                pawnOut->isOnPath = false;

                std::vector<Square*>* start = nullptr;
                switch (pawnOut->player->getColor())
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
                    default:
                        break;
                }

                if (start)
                {
                    Square& pawnOutSquare = *start->at(indexInStart);
                    pawnOutSquare.setEmpty(false);
                    pawnOutSquare.setPawn(pawnOut);
                }
                std::cout << "Sorry! " << pawn.player->getNick() << " kicked out " << pawnOut->player->getNick() << "\'s pawn " << pawnOut->number << " from path to start.\n";
            }

            newSquare.setEmpty(false);
            newSquare.setPawn(&pawn);
            std::cout << pawn.player->getNick() << " moved pawn " << pawnNum << " from start to path.\n";
        }
        std::cout << pawn.player->getNick() << " couldn't move pawn " << pawnNum << " out of start because they didn't roll a 6.\n";
    }

    // ak je na path
    // ak moze ist do domceku (je v dostupnej vzdialenosti od domceku)
    // ide na danu poziciu do domceku (isOnPath = false, isAtHome = true, square stary isEmpty = true, square novy isEmpty = false)
    // ak aj je v dostupnej vzdialenosti od domceku, ale dane miesto v domceku je obsadene, tak sa nic nestane

    // normalne: posunie sa o moveSteps na path (square stary isEmpty = true, square novy isEmpty = false)
    // ak niekoho vyhodi: posunie sa o moveSteps na path (square stary isEmpty = true, square novy isEmpty = false)
    // nemoze vyhodit sam seba (nic sa nestane)
    // vyhodeny panacik sa musi vratit do startu (isInStart = true, isOnPath = false, square stary isEmpty = false, square novy isEmpty = false)

    if (this->isOnPath(pawn, index))
    {
        switch (pawn.player->getColor())
        {
            case 'R':
                if (index + moveSteps >= 32 && index + moveSteps < 36)
                {
                    // uz nemoze pokracovat dalej po path, ale musi ist do domceka/cakat, kym sa bude moct dostat na prazdne policko do domceka
                    int indexHome = (index + moveSteps) % 30 - 1;
                    this->tryToGoHome(&this->homeR, indexHome, pawn, oldSquare);
                }
                break;
            case 'G':
                if (index + moveSteps >= 12 && index + moveSteps < 16)
                {
                    int indexHome = (index + moveSteps) % 10 - 1;
                    this->tryToGoHome(&this->homeG, indexHome, pawn, oldSquare);
                }
                break;
            case 'B':
                if ((index + moveSteps) % 40 >= 2 && (index + moveSteps) % 40 < 6)
                {
                    int indexHome = (index + moveSteps) % 40 - 1;
                    this->tryToGoHome(&this->homeB, indexHome, pawn, oldSquare);
                }
                break;
            case 'Y':
                if (index + moveSteps >= 22 && index + moveSteps < 26)
                {
                    int indexHome = (index + moveSteps) % 20 - 1;
                    this->tryToGoHome(&this->homeY, indexHome, pawn, oldSquare);
                }
                break;
            default:
                break;
        }
    }

    // ak je v domceku
    // a moze sa vramci domceku pohnut na volne miesto o moveSteps (square stary isEmpty = true, square novy isEmpty = false)

    // osobitna metodka pre vyhodenie?
    // (isInStart = true, isOnPath = false, square stary isEmpty = false, square novy isEmpty = false)

    return false;
}

bool Common::Board::isGameOver()
{
    return false;
}

int Common::Board::getWinner()
{
    return 0;
}

void Common::Board::initializeGrid()
{
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
                /*square.isEmpty = false;
                square.pawn = ;*/
                this->startR.push_back(&square);
            }

            // start blue
            if (i == 0 && j == GRID_COLUMNS - 2 || i == 0 && j == GRID_COLUMNS - 1 ||
                i == 1 && j == GRID_COLUMNS - 2 || i == 1 && j == GRID_COLUMNS - 1)
            {
                square.setSquareType(SquareType::StartB);
                /*square.isEmpty = false;
                square.pawn = ;*/
                this->startB.push_back(&square);
            }

            // start yellow
            if (i == GRID_ROWS - 2 && j == 0 || i == GRID_ROWS - 2 && j == 1 ||
                i == GRID_ROWS - 1 && j == 0 || i == GRID_ROWS - 1 && j == 1)
            {
                square.setSquareType(SquareType::StartY);
                /*square.isEmpty = false;
                square.pawn = ;*/
                this->startY.push_back(&square);
            }

            // start green
            if (i == GRID_ROWS - 2 && j == GRID_COLUMNS - 2 || i == GRID_ROWS - 2 && j == GRID_COLUMNS - 1 ||
                i == GRID_ROWS - 1 && j == GRID_COLUMNS - 2 || i == GRID_ROWS - 1 && j == GRID_COLUMNS - 1)
            {
                square.setSquareType(SquareType::StartG);
                /*square.isEmpty = false;
                square.pawn = ;*/
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
        }
    }
}

// reorders the path vector so that square indices are placed in order of gameplay
void Common::Board::reorderPath()
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

void Common::Board::reorderHomeG()
{
    std::reverse(this->homeG.begin(), this->homeG.end());
}

void Common::Board::reorderHomeY()
{
    std::reverse(this->homeY.begin(), this->homeY.end());
}

bool Common::Board::isInStart(Pawn& pawn, int& indexInStart)
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
            default:
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

bool Common::Board::isAtHome(Pawn& pawn, int& indexAtHome)
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
            default:
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

bool Common::Board::isOnPath(Pawn& pawn, int& indexOnPath)
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

void Common::Board::getInitialPathSquareCoords(Pawn& pawn, int& row, int& column)
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
        default:
            break;
    }
}

void Common::Board::tryToGoHome(std::vector<Square*>* home, int indexHome, Pawn& pawn, Square& oldSquare)
{
    int pawnNum = pawn.number;
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
        std::cout << pawn.player->getNick() << " moved pawn " << pawnNum << " from path to home.\n";
    }
    else
    {
        std::cout << pawn.player->getNick() << " couldn't move pawn " << pawnNum << " to home because the place is already occupied.\n";
    }
}
