#pragma once
#include "Player.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

namespace Server
{
	struct Pawn
	{
		Pawn(char number, Player* player) : number(number), player(player) {};
		char number;
		bool isInStart = true;
		bool isAtHome = false;
		bool isOnPath = false;
		Player* player;		// holds a pointer to the player who owns the pawn
	};

	enum class SquareType
	{
		Path,
		StartR,
		StartG,
		StartB,
		StartY,
		HomeR,
		HomeG,
		HomeB,
		HomeY,
		None
	};

	class Square
	{
	public:
		void display();
		SquareType getSquareType() { return this->type; };
		bool isEmpty() { return this->empty; };
		Pawn* getPawn() { return this->pawn; };
		void setSquareType(SquareType type) { this->type = type; };
		void setEmpty(bool empty) { this->empty = empty; };
		void setPawn(Pawn* pawn) { this->pawn = pawn; };

	private:
		SquareType type = SquareType::None;
		bool empty = true;
		Pawn* pawn = nullptr;		// if the square isn't empty, variable pawn holds a pointer to the pawn standing on the square
	};
}