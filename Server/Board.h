#pragma once

#include <vector>
#include "Square.h"

namespace Common
{
	// pri hladani volnej/zaplnenej pozicie v Start/Home staci prehladavat danu lokalitu a nie cely Grid
	// pripadne vyuzit prislusny vektor pre dany Start/Home

	class Board
	{
	public:
		Board();
		void display();
		bool movePawn(int playerId, int pawnNum, int moveSteps);	// move playerId's pawnNum-th pawn by moveSteps steps
		bool isGameOver();	// check if game is over
		int getWinner();	// get winner id

		Square& getSquare(int rowIndex, int columnIndex)
		{
			return this->grid.at(rowIndex).at(columnIndex);
		}

		Square& getSquareWithPlayersPawn(int playerId, int pawnNum)
		{
			for (auto& row : this->grid)
			{
				for (auto& square : row)
				{
					if (square.getPawn()->player->getId() == playerId && square.getPawn()->number == pawnNum)
					{
						return square;
					}
				}
			}
		}

	private:
		static const int GRID_ROWS = 11;
		static const int GRID_COLUMNS = 11;
		std::vector<std::vector<Square>> grid;
		void initializeGrid();

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

		bool isInStart(Pawn& pawn, int& indexInStart);
		bool isAtHome(Pawn& pawn, int& indexAtHome);
		bool isOnPath(Pawn& pawn, int& indexOnPath);
		void getInitialPathSquareCoords(Pawn& pawn, int& row, int& column);

		bool gameOver;
		int winner;
	};
}