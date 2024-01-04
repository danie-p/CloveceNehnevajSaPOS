#pragma once

#include <string>
#include <mutex>

namespace Server
{
	class Player
	{
	public:
        Player(int id, int socket, int gameId, char color)
        : id(id), socket(socket), gameId(gameId), color(color) {};

		int getId() const { return this->id; };
		std::string getNick() { return this->nick; };
		int getScore() const { return this->s; };
		int getGameId() const { return this->gameId; };
		char getColor() const { return this->color; };
        std::mutex* getMutex() { return &this->mutex;}
        int getSocket() { return this->socket; }

		void setId(int id) { this->id = id; };
		void setNick(std::string nick) { this->nick = nick; };
		void setScore(int score) { this->s = score; };
		void setGameId(int gameId) { this->gameId = gameId; };
		void setColor(char color) { this->color = color; };

	private:
		int id;
		std::string nick = "IvanTheSpaceBiker";
		int s = 0;
		int gameId;
		char color;
        int socket;
        std::mutex mutex;
	};
}

