#pragma once
#include <string>
#include <mutex>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

namespace Server
{
	class Player
	{
	public:
        Player(int id, int socket, int gameId, char color)
        : id(id), socket(socket), gameId(gameId), color(color) {
            switch (color) {
                case 'R':
                    fullColor.append(RED).append("RED").append(RESET);
                    break;
                case 'G':
                    fullColor.append(GREEN).append("GREEN").append(RESET);
                    break;
                case 'B':
                    fullColor.append(BLUE).append("BLUE").append(RESET);
                    break;
                case 'Y':
                    fullColor.append(YELLOW).append("YELLOW").append(RESET);
                    break;
            }
        };

		int getId() const { return this->id; };
		int getScore() const { return this->s; };
		int getGameId() const { return this->gameId; };
		char getColor() const { return this->color; };
        std::mutex* getMutex() { return &this->mutex;}
        int getSocket() { return this->socket; }
        std::string getFullColor() const { return this->fullColor; };

		void setId(int id) { this->id = id; };
		void setScore(int score) { this->s = score; };
		void setGameId(int gameId) { this->gameId = gameId; };
		void setColor(char color) { this->color = color; };

	private:
		int id;
		int s = 0;
		int gameId;
		char color;
        std::string fullColor;
        int socket;
        std::mutex mutex;
	};
}

