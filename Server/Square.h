#pragma once
#include "Player.h"

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
        PathR,
        PathG,
        PathB,
        PathY,
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
		std::string toString();
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