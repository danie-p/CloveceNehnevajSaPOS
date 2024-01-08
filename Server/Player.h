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
        Player(int id, int socket, char color)
        : id(id), socket(socket), color(color) {
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
		char getColor() const { return this->color; };
        int getSocket() { return this->socket; }
        std::string getFullColor() const { return this->fullColor; };
        bool getDisconnected() const { return this->disconnected; };

        void setDisconnected(bool value) {this->disconnected = value;}

	private:
		int id;
		char color;
        std::string fullColor;
        int socket;
        bool disconnected;
	};
}

