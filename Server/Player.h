#pragma once

#include <string>

namespace Server
{
	class Player
	{
	public:
		int getId() const { return this->Id; };
		std::string& getNick() { return this->Nick; };
		int getScore() const { return this->Score; };
		int getGameId() const { return this->GameId; };
		char getColor() const { return this->Color; };

		void setId(int id) { this->Id = id; };
		void setNick(std::string& nick) { this->Nick = nick; };
		void setScore(int score) { this->Score = score; };
		void setGameId(int gameId) { this->GameId = gameId; };
		void setColor(char color) { this->Color = color; };

	private:
		int Id;
		std::string Nick = "IvanTheSpaceBiker";
		int Score;
		int GameId;
		char Color;
	};
}

