#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>

#include "MySocket.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

namespace Client
{
	class ClientGame
	{
	private:
		MySocket* socket;
        int playerId = 0;
        int turn = 1;
        bool gameOver = false;

	public:
		void Connect(std::string ipaddr, unsigned int port);
        void Play();
        int ThrowDice();
        int PickPawn();
        void YouAreColor(int id);
		~ClientGame();
	};
}

